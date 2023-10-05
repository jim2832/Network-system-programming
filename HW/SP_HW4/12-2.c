static char *WhatString[] = {
    "@(#)pstree $Revision: 2.33-scc $ by Fred Hucht (C) 1993-2007, SCC (C) "
    "2021",
    "@(#)EMail: fred AT thp.Uni-Duisburg.de, zxc25077667 AT pm.me",
    "$Id: pstree.c,v 2.33-scc 2021-10-23 22:12:39+08 SCC Exp $"};

#define MAXLINE 8192

#include <glob.h>
#include <sys/stat.h>
#define PSCMD "ps -eo uid,pid,ppid,pgid,args"
#define PSFORMAT "%ld %ld %ld %ld %[^\n]"
#define PSVARS &P[i].uid, &P[i].pid, &P[i].ppid, &P[i].pgid, P[i].cmd

#include <pwd.h> /* For getpwnam() */
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* For str...() */
#include <sys/types.h>
#include <unistd.h> /* For getopt() */

#include <sys/ioctl.h> /* For TIOCGSIZE/TIOCGWINSZ */
/* #include <termios.h> */

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

struct TreeChars {
    char *s2,  /* SS String between header and pid */
        *p,    /* PP dito, when parent of printed childs */
        *pgl,  /* G  Process group leader */
        *npgl, /* N  No process group leader */
        *barc, /* C  bar for line with child */
        *bar,  /* B  bar for line without child */
        *barl, /* L  bar for last child */
        *sg,   /*    Start graphics (alt char set) */
        *eg,   /*    End graphics (alt char set) */
        *init; /*    Init string sent at the beginning */
};

/* Example:
 * |-+- 01111 ...        CPPN 01111 ...
 * | \-+=   01112 ...    B LPPG 01112 ...
 * |   |--= 01113 ...    B   CSSG 01113 ...
 * |   \--= 01114 ...    B   LSSG 01114 ...
 * \--- 01115 ...        LSSN 01115 ...
 */

enum { G_ASCII = 0, G_PC850 = 1, G_VT100 = 2, G_UTF8 = 3, G_LAST };

/* VT sequences contributed by Randall Hopper <rhh AT ct.picker.com> */
/* UTF8 sequences contributed by Mark-Andre Hopf <mhopf AT mark13.org> */
static struct TreeChars TreeChars[] =
    {
        /* SS          PP          G       N       C       B       L      sg
           eg      init */
        {"--", "-+", "=", "-", "|", "|", "\\", "", "", ""}, /*Ascii*/
        {"\304\304", "\304\302", "\372", "\304", "\303", "\263", "\300", "", "",
         ""}, /*Pc850*/
        {"qq", "qw", "`", "q", "t", "x", "m", "\016", "\017",
         "\033(B\033)0"}, /*Vt100*/
        {"\342\224\200\342\224\200",
         /**/ "\342\224\200\342\224\254",
         /**/ "=",
         /**/ "\342\224\200",
         /**/ "\342\224\234",
         /**/ "\342\224\202",
         /**/ "\342\224\224",
         /**/ "", "", ""} /*UTF8*/
},
                        *C;

int MyPid, NProc, Columns, RootPid;
short showall = TRUE, soption = FALSE, Uoption = FALSE, Hoption = FALSE,
      noption = FALSE, roption = FALSE;
char *name = "", *str = NULL, *Progname;
long ipid = -1;
char *input = NULL;

int atLdepth = 0;    /* LOPTION - track how deep in the print chain we are */
int maxLdepth = 100; /* LOPTION - will be changed by -l n option */

struct Proc {
    long uid, pid, ppid, pgid;
    char name[32], cmd[MAXLINE];
    int print;
    int selected;
    int hidden;
    long parent, child, sister;
    unsigned long thcount;
} *P;


#define NUMUN 128
void uid2user(uid_t uid, char *name, int len) {
    static struct un_ {
        uid_t uid;
        char name[32];
    } un[NUMUN];
    static short n = 0;
    short i;
    char uid_name[32];
    char *found;

    for (i = n - 1; i >= 0 && un[i].uid != uid; i--);

    if (i >= 0) { /* found locally */
        found = un[i].name;
    }
    else {
        struct passwd *pw = getpwuid(uid);
        if (pw) {
            found = pw->pw_name;
        }
        else {
            /* fix by Stan Sieler & Philippe Torche */
            snprintf(uid_name, sizeof(uid_name), "#%d", uid);
            found = uid_name;
        }
        if (n < NUMUN) {
            un[n].uid = uid;
            strncpy(un[n].name, found, 9);
            un[n++].name[8] = '\0';
        }
    }
    strncpy(name, found, len);
    name[len - 1] = '\0';
}


int GetProcessesDirect() {
    glob_t globbuf;
    unsigned int i, j;

    glob("/proc/[0-9]*", GLOB_NOSORT, NULL, &globbuf);

    P = calloc(globbuf.gl_pathc, sizeof(struct Proc));
    if (P == NULL) {
        fprintf(stderr, "Problems with malloc.\n");
        exit(1);
    }

    for (i = j = 0; i < globbuf.gl_pathc; i++) {
        char *pdir, name[32];
        int c;
        FILE *tn;
        int k = 0;

        pdir = globbuf.gl_pathv[globbuf.gl_pathc - i - 1];

        /* if processes change their UID this change is only reflected in the
         * owner of pdir. fixed since version 2.36 */
        {
            struct stat st;
            if (stat(pdir, &st) != 0) { /* get uid */
                continue;               /* process vanished since glob() */
            }
            P[j].uid = st.st_uid;
            uid2user(P[j].uid, P[j].name, sizeof(P[j].name));
        }

        snprintf(name, sizeof(name), "%s%s",
                 globbuf.gl_pathv[globbuf.gl_pathc - i - 1], "/stat");
        tn = fopen(name, "r");
        if (tn == NULL)
            continue; /* process vanished since glob() */
        fscanf(tn, "%ld %s %*c %ld %ld", &P[j].pid, P[j].cmd, &P[j].ppid,
               &P[j].pgid);
        fclose(tn);
        P[j].thcount = 1;

        snprintf(name, sizeof(name), "%s%s",
                 globbuf.gl_pathv[globbuf.gl_pathc - i - 1], "/cmdline");
        tn = fopen(name, "r");
        if (tn == NULL)
            continue; /* process vanished since glob() */
        while (k < MAXLINE - 1 && EOF != (c = fgetc(tn))) {
            P[j].cmd[k++] = c == '\0' ? ' ' : c;
        }
        if (k > 0)
            P[j].cmd[k] = '\0';
        fclose(tn);

        P[j].parent = P[j].child = P[j].sister = -1;
        P[j].print = FALSE;
        j++;
    }

    globfree(&globbuf);
    return j;
}


int GetProcesses() {
    FILE *tn;
    int i = 0;
    char line[MAXLINE], command[] = PSCMD;

    /* file read code contributed by Paul Kern <pkern AT utcc.utoronto.ca> */
    if (input != NULL) {
        if (strcmp(input, "-") == 0)
            tn = stdin;

        else if (NULL == (tn = fopen(input, "r"))) {
            perror(input);
            exit(1);
        }
    }

    P = malloc(sizeof(struct Proc));
    if (P == NULL) {
        fprintf(stderr, "Problems with malloc.\n");
        exit(1);
    }

    while (fgets(line, MAXLINE, tn) != NULL) {
        int len, num;
        len = strlen(line);

        if (len == MAXLINE - 1) { /* line too long, drop remaining stuff */
            char tmp[MAXLINE];
            while (MAXLINE - 1 == strlen(fgets(tmp, MAXLINE, tn)))
                ;
        }

        P = realloc(P, (i + 1) * sizeof(struct Proc));
        if (P == NULL) {
            fprintf(stderr, "Problems with realloc.\n");
            exit(1);
        }

        memset(&P[i], 0, sizeof(*P));

        num = sscanf(line, PSFORMAT, PSVARS);

        P[i].parent = P[i].child = P[i].sister = -1;
        P[i].print = FALSE;
        i++;
    }

    if (input != NULL)
        fclose(tn);
    else
        pclose(tn);

    return i;
}


int GetRootPid() {
    int me;

    for (me = 0; me < NProc; me++) {
        if (P[me].pid == 1)
            return P[me].pid;
    }

    /* PID == 1 not found, so we'll take process with PPID == 0
     * Fix for TRU64 TruCluster with uniq PIDs
     * reported by Frank Parkin <fparki AT acxiom.co.uk>
     * re-reported by Eric van Doorn <Eric.van.Doorn AT isc.politie.nl>,
     * because fix was not published by me :-/ */
    for (me = 0; me < NProc; me++) {
        if (P[me].ppid == 0)
            return P[me].pid;
    }

    /* OK, still nothing found. Maybe it is FreeBSD and won't show foreign
     * processes. So we also accept PPID == 1 */
    for (me = 0; me < NProc; me++) {
        if (P[me].ppid == 1)
            return P[me].pid;
    }

    /* Still nothing. Maybe it is something like Solaris Zone. We'll take
     * the process with PID == PPID */
    for (me = 0; me < NProc; me++) {
        if (P[me].pid == P[me].ppid)
            return P[me].pid;
    }

    /* Should not happen */
    fprintf(stderr,
            "%s: No process found with PID == 1 || PPID == 0 || PPID == 1\n"
            "          || PID == PPID, contact author.\n",
            Progname);

    exit(1);
}


int get_pid_index(long pid) {
    int me;
    for (me = NProc - 1; me >= 0 && P[me].pid != pid; me--); /* Search process */

    return me;
}


#define EXIST(idx) ((idx) != -1)


void MakeTree() {
    /* Build the process hierarchy. Every process marks itself as first child
     * of it's parent or as sister of first child of it's parent */
    int me;

    for (me = 0; me < NProc; me++) {
        int parent;
        parent = get_pid_index(P[me].ppid);
        if (parent != me && parent != -1) { /* valid process, not me */
            P[me].parent = parent;
            if (P[parent].child == -1) /* first child */
                P[parent].child = me;
            else {
                int sister;
                for (sister = P[parent].child; EXIST(P[sister].sister);
                     sister = P[sister].sister)
                    ;
                P[sister].sister = me;
            }
        }
    }
}


void MarkChildren(int me) {
    int child;
    P[me].print = TRUE;
    for (child = P[me].child; EXIST(child); child = P[child].sister){
        MarkChildren(child);
    }
}


void MarkProcs() {
    int me;
    for (me = 0; me < NProc; me++) {
        if (showall) {
            P[me].print = TRUE;
        }
        else {
            int parent;
            if (0 == strcmp(P[me].name, name)                   /* for -u */
                || (Uoption && 0 != strcmp(P[me].name, "root")) /* for -U */
                || P[me].pid == ipid                            /* for -p */
                || (soption && NULL != strstr(P[me].cmd, str) &&
                    P[me].pid != MyPid) /* for -s */
            ) {
                /* Mark parents */
                for (parent = P[me].parent; EXIST(parent);
                     parent = P[parent].parent) {
                    P[parent].print = TRUE;
                }
                /* Mark children */
                MarkChildren(me);
            }
        }
    }
}


void DropProcs() {
    int me;
    for (me = 0; me < NProc; me++){
        if (P[me].print) {
            int child, sister;

            /* Drop children that won't print */
            for (child = P[me].child; EXIST(child) && !P[child].print;
                 child = P[child].sister);
            P[me].child = child;

            /* Drop sisters that won't print */
            for (sister = P[me].sister; EXIST(sister) && !P[sister].print;
                 sister = P[sister].sister);
            P[me].sister = sister;
        }
    }
}


void PrintTree(int idx, const char *head) {
    char nhead[MAXLINE], out[4 * MAXLINE], thread[16] = {'\0'};
    int child;

    if (head[0] == '\0' && !P[idx].print)
        return;

    if (P[idx].thcount > 1)
        snprintf(thread, sizeof(thread), "[%ld]", P[idx].thcount);

    if (atLdepth == maxLdepth)
        return; /* LOPTION */
    ++atLdepth; /* LOPTION */

    snprintf(out, sizeof(out),
             "%s%s%s%s%s%s %05ld %s %s%s" /*" (ch=%d, si=%d, pr=%d)"*/, C->sg,
             head,
             head[0] == '\0'        ? ""
             : EXIST(P[idx].sister) ? C->barc
                                    : C->barl,
             EXIST(P[idx].child) ? C->p : C->s2,
             P[idx].pid == P[idx].pgid ? C->pgl : C->npgl, C->eg, P[idx].pid,
             P[idx].name, thread, P[idx].cmd
             /*,P[idx].child,P[idx].sister,P[idx].print*/);

    out[Columns - 1] = '\0';
    puts(out);

    /* Process children */
    snprintf(nhead, sizeof(nhead), "%s%s ", head,
             head[0] == '\0'        ? ""
             : EXIST(P[idx].sister) ? C->bar
                                    : " ");

    for (child = P[idx].child; EXIST(child); child = P[child].sister) {
        PrintTree(child, nhead);
    }

    --atLdepth; /* LOPTION */
}


void Usage() {
    fprintf(
        stderr,
        "%s\n"
        "%s\n\n"
        "Usage: %s "
        "[-f file] [-g n] [-l n] [-u user] [-U] [-s string] [-p pid] [-w] [pid "
        "...]\n"
        "   -f file   read input from <file> (- is stdin) instead of running\n"
        "             \"%s\"\n"
        "   -g n      use graphics chars for tree. n=1: IBM-850, n=2: VT100, "
        "n=3: UTF-8\n"
        "   -l n      print tree to n level deep\n"
        "   -u user   show only branches containing processes of <user>\n"
        "   -U        don't show branches containing only root processes\n"
        "   -s string show only branches containing process with <string> in "
        "commandline\n"
        "   -p pid    show only branches containing process <pid>\n"
        "   -w        wide output, not truncated to window width\n"
        "   pid ...   process ids to start from, default is 1 (probably "
        "init)\n",
        WhatString[0] + 4, WhatString[1] + 4, Progname, PSCMD);

    exit(1);
}

int main(int argc, char **argv) {
    extern int optind;
    extern char *optarg;
    int ch;
    long pid;
    int graph = G_UTF8, wide = FALSE;

    C = &TreeChars[graph];

    Progname = strrchr(argv[0], '/');
    Progname = (NULL == Progname) ? argv[0] : Progname + 1;

    while ((ch = getopt(argc, argv, "df:g:hl:p:s:u:Uw?Hnr")) != EOF)
        switch (ch) {
        case 'f':
            input = optarg;
            break;
        case 'g':
            graph = atoi(optarg);
            if (graph < 0 || graph >= G_LAST) {
                fprintf(stderr, "%s: Invalid graph parameter.\n", Progname);
                exit(1);
            }
            C = &TreeChars[graph];
            break;
        case 'l':                     /* LOPTION */
            maxLdepth = atoi(optarg); /* LOPTION */
            if (maxLdepth < 1)
                maxLdepth = 1; /* LOPTION */
            break;             /* LOPTION */
        case 'H':
            Hoption = TRUE;
            break;
        case 'n':
            noption = TRUE;
            break;
        case 'r':
            roption = TRUE;
            break;
        case 'p':
            showall = FALSE;
            ipid = atoi(optarg);
            break;
        case 's':
            showall = FALSE;
            soption = TRUE;
            str = optarg;
            break;
        case 'u':
            showall = FALSE;
            name = optarg;
            if (!(getpwnam(name))) {
                fprintf(stderr, "%s: User '%s' does not exist.\n", Progname,
                        name);
                exit(1);
            }
            break;
        case 'U':
            showall = FALSE;
            Uoption = TRUE;
            break;
        case 'w':
            wide = TRUE;
            break;
        case 'h':
        case '?':
        default:
            Usage();
            break;
        }

    NProc = (input == NULL) ? GetProcessesDirect() : GetProcesses();
    RootPid = GetRootPid();
    MyPid = getpid();

    if (wide)
        Columns = MAXLINE - 1;
    else {
        #if defined(HAS_TERMDEF)
                Columns = atoi((char *)termdef(fileno(stdout), 'c'));
        #elif defined(TIOCGWINSZ)
                struct winsize winsize;
                ioctl(fileno(stdout), TIOCGWINSZ, &winsize);
                Columns = winsize.ws_col;
        #elif defined(TIOCGSIZE)
                struct ttysize ttysize;
                ioctl(fileno(stdout), TIOCGSIZE, &ttysize);
                Columns = ttysize.ts_cols;
        #else
                char *env = getenv("COLUMNS");
                Columns = env ? atoi(env) : 80;
        #endif
    }
    if (Columns == 0)
        Columns = MAXLINE - 1;

    printf("%s", C->init);

    Columns += strlen(C->sg) + strlen(C->eg); /* Don't count hidden chars */

    if (Columns >= MAXLINE)
        Columns = MAXLINE - 1;

    MakeTree();
    MarkProcs();
    DropProcs();

    if (argc == optind) { /* No pids */
        PrintTree(get_pid_index(RootPid), "");
    }
    else{
        while (optind < argc) {
            int idx;
            pid = (long)atoi(argv[optind]);
            idx = get_pid_index(pid);
            if (idx > -1)
                PrintTree(idx, "");
            optind++;
        }
    }

    free(P);
    return 0;
}

/*
 * $Log: pstree.c,v $

 * Revision 2.331  2023-10-05 17:08:30+01  Jim Lee
 * Make some revisions to meet the basic need of 'pstree' command and formatting pstree.c
 *
 * Revision 2.33  2009-11-10 22:12:39+01  fred
 * Added UTF8, enlarged MAXLINE
 *
 * Revision 2.32  2007-10-26 21:39:50+02  fred
 * Added option -l provided by Michael E. White <mewhite AT us.ibm.com>
 *
 * Revision 2.31  2007-06-08 17:45:23+02  fred
 * Fixed problem with users with long login name (Reported by Oleg A. Mamontov)
 *
 * Revision 2.30  2007-05-10 23:13:04+02  fred
 * *** empty log message ***
 *
 * Revision 2.29  2007-05-10 22:37:13+02  fred
 * Added fix for Solaris Zone and bug fix from Philippe Torche
 *
 * Revision 2.28  2007-05-10 22:01:07+02  fred
 * Added new determination of window width
 *
 * Revision 2.27  2005-04-08 22:08:45+02  fred
 * Also accept PPID==1 if nothing else is found. Should fix problem with
 * FreeBSD and security.bsd.see_other_uids=0.
 *
 * Revision 2.26  2004-10-15 13:59:03+02  fred
 * Fixed small bug with char/int variable c
 * reported by Tomas Dvorak <tomas_dvorak AT mailcan.com>
 *
 * Revision 2.25  2004-05-14 16:41:39+02  fred
 * Added workaround for spurious blank lines in ps output under AIX 5.2
 * reported by Dean Rowswell <rowswell AT ca.ibm.com>
 *
 * Revision 2.24  2004-04-14 09:10:29+02  fred
 * *** empty log message ***
 *
 * Revision 2.23  2004-02-16 10:55:20+01  fred
 * Fix for zombies (pid == 0) under FreeBSD
 *
 * Revision 2.22  2003-12-12 10:58:46+01  fred
 * Added support for TRU64 v5.1b TruCluster
 *
 * Revision 2.21  2003-10-06 13:55:47+02  fred
 * Fixed SEGV under Linux when process table changes during run
 *
 * Revision 2.20  2003-07-09 20:07:29+02  fred
 * cosmetic
 *
 * Revision 2.19  2003/05/26 15:33:35  fred
 * Merged FreeBSD, (Open|Net)BSD; added Darwin (APPLE), fixed wide output
 * in FreeBSD
 *
 * Revision 2.18  2003/03/13 18:53:22  fred
 * Added getenv("COLUMNS"), cosmetic changes
 *
 * Revision 2.17  2001/12/17 12:18:02  fred
 * Changed ps call to something like ps -eo uid,pid,ppid,pgid,args under
 * AIX and Linux, workaround for AIX 5L.
 *
 * Revision 2.17  2001-12-13 08:27:00+08  chris
 * Added workaround for AIX Version >= 5
 *
 * Revision 2.16  2000-03-01 10:42:22+01  fred
 * Added support for thread count (thcount) in other OSs than AIX
 *
 * Revision 2.15  2000-03-01 10:18:56+01  fred
 * Added process group support for{Net|Open}BSD following a suggestion
 * by Ralf Meyer <ralf AT thp.Uni-Duisburg.de>
 *
 * Revision 2.14  1999-03-22 20:45:02+01  fred
 * Fixed bug when line longer than MAXLINE, set MAXLINE=512
 *
 * Revision 2.13  1998-12-17 19:31:53+01  fred
 * Fixed problem with option -f when input file is empty
 *
 * Revision 2.12  1998-12-07 17:08:59+01  fred
 * Added -f option and sun 68000 support by Paul Kern
 * <pkern AT utcc.utoronto.ca>
 *
 * Revision 2.11  1998-05-23 13:30:28+02  fred
 * Added vt100 sequences, NetBSD support
 *
 * Revision 2.10  1998-02-02 15:04:57+01  fred
 * Fixed bug in MakeTree()/get_pid_index() when parent doesn't
 * exist. Thanks to Igor Schein <igor AT andrew.air-boston.com> for the bug
 * report.
 *
 * Revision 2.9  1998-01-07 16:55:26+01  fred
 * Added support for getprocs()
 *
 * Revision 2.9  1998-01-06 17:13:19+01  fred
 * Added support for getprocs() under AIX
 *
 * Revision 2.8  1997-10-22 15:09:39+02  fred
 * Cosmetic
 *
 * Revision 2.7  1997-10-22 15:01:40+02  fred
 * Minor changes in getprocs for AIX
 *
 * Revision 2.6  1997/10/16 16:35:19  fred
 * Added uid2name() caching username lookup, added patch for Solaris 2.x
 *
 * Revision 2.5  1997-02-05 14:24:53+01  fred
 * return PrintTree when nothing to do.
 *
 * Revision 2.4  1997/02/05 09:54:08  fred
 * Fixed bug when P[i].cmd is empty
 *
 * Revision 2.3  1997-02-04 18:40:54+01  fred
 * Cosmetic
 *
 * Revision 2.2  1997-02-04 14:11:17+01  fred
 * *** empty log message ***
 *
 * Revision 2.1  1997-02-04 13:55:14+01  fred
 * Rewritten
 *
 * Revision 1.13  1997-02-04 09:01:59+01  fred
 * Start of rewrite
 *
 * Revision 1.12  1996-09-17 21:54:05+02  fred
 * *** empty log message ***
 *
 * Revision 1.11  1996-09-17 21:52:52+02  fred
 * revision added
 *
 * Revision 1.10  1996-09-17 21:45:35+02  fred
 * replace \n and \t with ? in output
 *
 * Revision 1.4  1996-09-17 21:43:14+02  fred
 * Moved under RCS, replace \n and \t with ?
 */