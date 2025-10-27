cmake_minimum_required(VERSION 3.28)
project(tweets_generator C)

set(CMAKE_C_STANDARD 99)

add_executable(tweets_generator tweets_generator.c markov_chain.c markov_chain.h linked_list.c
        linked_list.h justdoit_tweets.txt)
