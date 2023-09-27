while (token != NULL) {
        // Resize the array to hold more pointers
        newArgv = (char **)realloc(newArgv, (count + 1) * sizeof(char *));
        if (newArgv == NULL) {
            perror("Memory allocation error");
            exit(EXIT_FAILURE);
        }

        // Allocate memory for the token and copy it
        newArgv[count] = (char *)malloc(strlen(token) + 1);
        if (newArgv[count] == NULL) {
            perror("Memory allocation error");
            exit(EXIT_FAILURE);
        }
        strcpy(newArgv[count], token);

        token = strtok(NULL, delim); // Get the next token
		printf("[%d] : %s\n", count, token);
        count++;
    }

    // Resize the array to hold one more pointer for NULL termination
    newArgv = (char **)realloc(newArgv, (count + 1) * sizeof(char *));
    if (newArgv == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    newArgv[count] = NULL; // Null-terminate the array