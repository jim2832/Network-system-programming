#!/bin/bash

# compile
make

# test arrays
consumers=(10 100 1000)
rates=(300 500 800 1000)
buffer_sizes=(1 2 3 4 5 6 7 8 9 10)


echo Processing...


# 第一題
echo -e "rate\tconsumer\tloss_rate" > question1
for rate in "${rates[@]}"
do
    for consumer in "${consumers[@]}"
    do
        result=$(./main 1000 $rate $consumer 3)
        loss_rate=$(echo "$result" | grep -o 'Loss rate: [0-9.]\+%' | sed 's/Loss rate: //')
        if [ ${#rate} -eq 4 ]; then
            if [ ${#consumer} -eq 4 ]; then
                echo -e "$rate\t$consumer\t\t${loss_rate}" >> question1
            else
                echo -e "$rate\t$consumer\t\t\t${loss_rate}" >> question1
            fi
        else
            if [ ${#consumer} -eq 4 ]; then
                echo -e "$rate\t\t$consumer\t\t${loss_rate}" >> question1
            else
                echo -e "$rate\t\t$consumer\t\t\t${loss_rate}" >> question1
            fi
        fi
    done
done


# 第二題
echo -e "buffer_size\tconsumer\tloss_rate" > question2
for buffer_size in "${buffer_sizes[@]}"
do
    for consumer in "${consumers[@]}"
    do
        result=$(./main 1000 500 $consumer $buffer_size)
        loss_rate=$(echo "$result" | grep -o 'Loss rate: [0-9.]\+%' | sed 's/Loss rate: //')
        if [ ${#consumer} -eq 4 ]; then
            echo -e "$buffer_size\t\t\t$consumer\t\t${loss_rate}" >> question2
        else
            echo -e "$buffer_size\t\t\t$consumer\t\t\t${loss_rate}" >> question2
        fi
    done
done

# clean
make clean