#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STRING_SIZE 300
#define MAX_TOPIC_QUANTITY 100
#define MAX_NODE_QUANTITY 100

typedef struct topic{
    char name[MAX_STRING_SIZE];
    char type[MAX_STRING_SIZE];
    char publishers[MAX_NODE_QUANTITY][MAX_STRING_SIZE];
    char subscribers[MAX_NODE_QUANTITY][MAX_STRING_SIZE];
    unsigned int publisher_count;
    unsigned int subscriber_count;
} struct_topic;


int main()
{

  struct_topic topic_info[MAX_TOPIC_QUANTITY];
  
  FILE *topic_list_file;
  topic_list_file = popen("ros2 topic list", "r");
  if (topic_list_file == NULL) {
    printf("Failed to run command\n" );
    return 0;
  }


  
  /*
    Here we will execute a command to get all the topics.
    For each topic we need the name, type, publishers and subscribers.
  */
  char topic_name[MAX_STRING_SIZE];
  unsigned int current_topic_number=0;
  char excluded_topics[2][20] = {"/rosout","/parameter_events"};
  while (fgets(topic_name, sizeof(topic_name), topic_list_file) != NULL) {
    //printf("%s", topic_name);
    topic_name[strlen(topic_name)-1]='\0'; // remove trailing \n
    if(strcmp(topic_name,excluded_topics[0])==0 || strcmp(topic_name,excluded_topics[1])==0){
      continue;
    }
    sprintf(topic_info[current_topic_number].name,"%s", (const char *)topic_name);
    //printf("%s\n",topic_info[counter_for_topics].name);

    
    /* The popen function is used to open a shell, execute a command and output the results to a file. Read the man page for more information. */
    /* The commands inside popen retrieve information about a topic, in this case: Type,Publisher count,Node name, Node namespace and Subscription count:
    Details:
    1- Execute ros2 command ros2 to obtain information about the topic. Command: ros2 topic info /rosout -v 
    2- The result is given to awk command through a pipe. Command: |
    3- Command: awk -F': ' '/Type:|Publisher count:|Node name:|Node namespace:|Subscription count:/ { print $2 }'.
        The awk command searchs every_line and if finds any of the words between / /, it prints the second field.
        3.1 (-F': ' '): -F is used to choose a delimiter, that is, what separates a field (a word).
        ': ' was choosen because every variable can have multiple words separated by space and its ending is always ': '.
        That is: "word1 word2 word3 ... wordN: value of variable".
        If we had choosen just ' ', "Publisher count:" would have been split into 2 words "Publisher" and "count:" and
        we would not have the correct match.
        3.2 ('/Type:|Publisher count:|Node name:|Node namespace:|Subscription count:/): words to match
        3.3 ({ print $2 }') After the match, we print the value of the variable, the second field in this case: { print $2 }.
    */
    char command_string[500];
   
    sprintf(command_string,"ros2 topic info %s -v | awk -F': ' '/Type:|Publisher count:|Node namespace:|Node name:|Subscription count:/ { print $2 }'",topic_info[current_topic_number].name);
    //printf("%s\n", command_string);
    FILE *fp;
    fp = popen(command_string, "r");

    if (fp == NULL) {
      printf("Failed to run command\n" );
      return 0;
    }

    /*
    This is the sequence:Type,Publisher Count, Publisher Count*(Node namespace, Node name), Subscription Count, Subscription Count*(Node namespace, Node name)
    Node namespace and node name should be joined in a single string.
    */

    /* Read the output a line at a time - output it. */
    unsigned int i=0;
    char topic_line[MAX_STRING_SIZE];
    while (fgets(topic_line, sizeof(topic_line), fp) != NULL) {
      topic_line[strlen(topic_line)-1]='\0'; // remove trailing \n
      if(i==0){
        sprintf(topic_info[current_topic_number].type,"%s", (const char *)topic_line);
      } else if(i==1){
        topic_info[current_topic_number].publisher_count = atoi((const char *)topic_line);
      } else if(i - 2 < topic_info[current_topic_number].publisher_count){
        /* i=0 (type), i=1(publisher count), i=2 (first publisher), i=3 (second publisher) and so on.
          If publisher count = 3 | If Publisher count =0  
            2-2 < 2 => 0< 2 true  | 2-2 <0 => 0< false
            3-2 < 2 => 1<2 true
            4-2 < 2 => 2<2 false. */
        char node_namespace[MAX_STRING_SIZE];
        if(fgets(node_namespace, sizeof(node_namespace), fp) != NULL){
          node_namespace[strlen(node_namespace)-1]='\0'; // remove trailing \n
          sprintf(topic_info[current_topic_number].publishers[i-2],"%s%s", (const char *) node_namespace,(const char *)topic_line);
        }
      } else if(i - 2 == topic_info[current_topic_number].publisher_count){
        topic_info[current_topic_number].subscriber_count = atoi((const char *)topic_line);
      } else if(i - 3 -  topic_info[current_topic_number].publisher_count < topic_info[current_topic_number].subscriber_count){
          /* i=0 (type), i=1(publisher count), i=2 (first publisher), i=3 (second publisher) and so on.
          If publisher count = 2 and subscriber count = 2
          i=4, i-2 = 2 == publisher count, takes the subcribers amount.
          i=5, i - 3 - publisher count = 5-3-2 = 0, 0 < subscriber count => 0 < 2, true.
          i=6, 6-3-2  = 1, 1<2, true
          */
        char node_namespace[MAX_STRING_SIZE];
        if(fgets(node_namespace, sizeof(node_namespace), fp) != NULL){
          node_namespace[strlen(node_namespace)-1]='\0'; // remove trailing \n
          sprintf(topic_info[current_topic_number].subscribers[i - 3 - topic_info[current_topic_number].publisher_count],"%s%s", (const char *) node_namespace,(const char *)topic_line);
        }

        // sprintf(topic_info[current_topic_number].subscribers[i - 3 - topic_info[current_topic_number].publisher_count],"%s", (const char *)topic_line);
      }
     
      i++;
    }

    /* close */
    pclose(fp);
    current_topic_number++;
  }
  /* close */
  pclose(topic_list_file);

  //========================================= Writing the table with topics information in markdown format ============================================================
  //===================================================================================================================================================================
  /*
  This is how the table should look like.
|Number  | Topic                         | Type          | Publisher(s)           | Subscriber(s)        | Details                               |
|:---------:|  |:-----------------------------:|:-------------:|:----------------------:|:--------------------:|:-------------------------------------:|
| Topic number  | Topic name             | Type of topic | Name of publishers     |  Name of subscribers | More details about this topic         | */

  FILE *markdow_file;
  markdow_file = fopen("topic_information.md","w");
  char header[] = "\
| Number | Topic    | Type          | Publisher(s)           | Subscriber(s)            | Details                |\n\
|:-----------------:|:-----------------------------:|:-------------:|:------------------------:|:--------------------------:|:----------------------:|\n";
  fputs(header,markdow_file);
 
  for(unsigned int i=0;i<current_topic_number;i++){
    fprintf(markdow_file,"|%d|%s|%s|",i+1,topic_info[i].name,topic_info[i].type);
    for(unsigned int j=0;j<topic_info[i].publisher_count;j++){
      fprintf(markdow_file,"%s",topic_info[i].publishers[j]);
      if(j+1<topic_info[i].publisher_count){
        fprintf(markdow_file,", ");
      }
    }
    fprintf(markdow_file,"|");
    for(unsigned int j=0;j<topic_info[i].subscriber_count;j++){
      fprintf(markdow_file,"%s",topic_info[i].subscribers[j]);
      if(j+1<topic_info[i].subscriber_count){
        fprintf(markdow_file,", ");
      }
    }
    fprintf(markdow_file,"||\n");
  }
 fclose(markdow_file);

  return 0;
}