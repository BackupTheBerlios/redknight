typedef struct{
    int command;
    int time;
    int id;
}Q;
Q *Queue[20];
//void process_queue(Q * queue);
//void add_to_queue(int time, int command, int id);

/*Actions*/
#define RETURN_TO_POST 1
#define SIT 2
#define ATTACK 3  
#define RETURN_HOME 4
/*Timed Actions*/
#define WHEN_DONE_MOVING 1
#define WHEN_DONE_FIGHTING 2  
