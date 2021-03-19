#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h> 
#include <stdlib.h>
#include <string.h>

using namespace std;
#define MAX_SIZE 101  //maximum size of the array that will store Queue. 
int CPU = 0 ;

class ProcessData{
public:	
	int arrivalTime,burstTime,completitionTime,tempBurstTime,blockingTime,TAT ;
	char name[10];
	ProcessData(){
		arrivalTime = -1;
		burstTime = -1;
		completitionTime = -1;
		tempBurstTime = -1;
		blockingTime = -1 ;
		TAT = 0;
	}
	void set( int a, int b ){
		arrivalTime = a;
		burstTime = b;
		tempBurstTime = b;
		completitionTime = 0 ;
	}
	void config(){
		tempBurstTime = burstTime;
	}

	void clear(){
		arrivalTime = -1;
		burstTime = -1;
		completitionTime = -1;
		tempBurstTime = -1;
		blockingTime = -1;	
	}

	void setForExit(){
		completitionTime = CPU;
		TAT = completitionTime - arrivalTime ;
		tempBurstTime = -1;
	}


};
class Queue{
private:
	ProcessData A[MAX_SIZE];
	int front, rear; 
public:
	Queue(){ front = -1; rear = -1; }

	bool IsEmpty(){ return (front == -1 && rear == -1);  }
	bool IsFull(){	return (rear+1)%MAX_SIZE == front ? true : false; }
	void Enqueue(ProcessData x)	{
		if(IsFull())
		{
			cout<<"Error: Queue is Full\n";
			return;
		}
		if (IsEmpty())
		{ 
			front = rear = 0; 
		}
		else
		{
		    rear = (rear+1)%MAX_SIZE;
		}
		A[rear] = x;
	} 
	void Dequeue(){
		//cout<<"Dequeuing \n";
		if(IsEmpty())
		{
			cout<<"Error: Queue is Empty\n";
			return;
		}
		else if(front == rear ) 
		{
			rear = front = -1;
		}
		else
		{
			front = (front+1)%MAX_SIZE;
		}
	} 
	ProcessData Front(){
		if(front == -1)
		{
			cout<<"Error: cannot return front from empty queue\n";
		}
		return A[front];
	}
	
};


Queue readyqueue ;
Queue blockqueue ;	

ProcessData pd[10];
int size = 0 ;

void removeProcessFromArray( ProcessData p[], int &size,int i ){
	for ( int j = i ; j < size - 1 ; j++ )
		p[j] = p[j+1] ;
	size -- ;
}

void newState( int sign = 0 ) 
{ 

	if ( sign == 0 ){
	    char *c = (char *) calloc(100, sizeof(char));     
	    int file_desc = open("dup.txt", O_RDONLY);
	    if(file_desc < 0) 
	        printf("Error opening the file\n"); 
	    int copy_desc = dup(file_desc); 
	    int end =  read(copy_desc,c, 99); // reading whole data in buffer
	    c[end] = '\0'; 
	    
	    char line[50] = "";
	    int flag = 0;
	    for ( int i = 0 ; c[i] != '\0' ; i++ ){	
			if ( c[i] != '\n' )
				strncat(line,&c[i],1);
			else{			
				if ( flag == 0  ){
					flag ++  ;	
				}else{
					if ( flag == 1 ){
						strcpy( pd[size].name , line );
					}
					else if ( flag == 2 ){
						int i;
						sscanf(line, "%d", &i);
						pd[size].arrivalTime = i;// convert line string to int   	
					}
					else if ( flag == 3 ){
						int i;
						sscanf(line, "%d", &i);
						pd[size].burstTime = i;
						flag = 0 ;
						size ++ ;	
					}
					flag ++;
				}
				line[0] = '\0';	
			}
	    }
		
	    for ( int i = 0 ; i < size ; i++ )
	    	pd[i].config();
	    


	    for ( int i = 0 ; i < size ; i ++ ){
	    	if ( pd[i].arrivalTime == CPU ){
	    		readyqueue.Enqueue(pd[i]);
	    		removeProcessFromArray( pd,size,i);
	    		i -- ;
	    	}
	    }

///	    printf("%d\n%d\n",size,CPU );
	}else {


		for ( int i = 0 ; i < size ; i ++ ){
	    	if ( pd[i].arrivalTime == CPU ){
	    		readyqueue.Enqueue(pd[i]);
	    		removeProcessFromArray( pd,size,i);
	    		i -- ;
	    	}
	    }

       
	}

	return ;
} 


void exitState( ProcessData &p , int flag ){

	if ( flag == 0  ){ // write p to file using dup.

		p.setForExit();
	    int file_desc = open("output.txt", O_WRONLY | O_APPEND); 
	      
	    if(file_desc < 0) 
	        printf("Error opening the file\n"); 
	      
	    int copy_desc = dup(file_desc); 

	    write(copy_desc,p.name,5);
	    write(copy_desc,"\n",1);
	    char buf[6];
	    int s = sprintf(buf, "%d", p.arrivalTime);
	    write(copy_desc,buf,s);
	    write(copy_desc,"\n",1);
	    
	    buf[0] = '\0';
	    s = sprintf(buf, "%d", p.burstTime);
	    write(copy_desc,buf,s);
	    write(copy_desc,"\n",1);
	    
	    buf[0] = '\0';
	    s = sprintf(buf, "%d", p.TAT);
	    write(copy_desc,buf,s);
		write(copy_desc,"\n",1);
	 
		buf[0] = '\0';
	    s = sprintf(buf, "%d", p.TAT-p.burstTime);
	    write(copy_desc,buf,s);
		write(copy_desc,"\n",1);

	}

}


int readyState( ProcessData &p,int flag ){

	if ( flag == 0  ){//started
		p = readyqueue.Front();
		readyqueue.Dequeue();
	}

	if( flag == 2 ){
		readyqueue.Enqueue(p);
		p.clear();
	}


	// readu state will also check newly entered processes here.
	if ( flag == 1 ){	
		
		if ( !readyqueue.IsEmpty()  ){	
			p = readyqueue.Front();
			readyqueue.Dequeue();
			return 1;
		} 
	
	}

	return 0;
}



int generateRandom(int l, int u)
{
   srand(time(NULL));
   int r = rand() % ((u - l) + 1);
   return r+l;
}



int randomtime = 16;

void blockState( ProcessData &p, int f ){
	if ( f ==0 ){ // called by running to endue process
		p.blockingTime = CPU;
		blockqueue.Enqueue(p);
		p.clear();
	}else{

		if ( ! blockqueue.IsEmpty() ){

			if ( ( blockqueue.Front().blockingTime + randomtime ) == CPU ){
				randomtime = generateRandom(15,25);
				ProcessData q =blockqueue.Front(); 
				cout <<q.name <<" unblocked."<< "\n";
				readyState(q,2);
				blockqueue.Dequeue();

			}
				
		}
		
	}
	

}



int main (){
	
	
	// initializing cpu
	ProcessData currentlyrunningprocess;
	newState();
	readyState(currentlyrunningprocess,0);

	// this is running state.
	while(1)
	{
		


		blockState(currentlyrunningprocess,2);
		newState(1);


		if ( currentlyrunningprocess.tempBurstTime < 0 ){ 
			if(readyState(currentlyrunningprocess,1) == 0)
				cout << "CPU running idle\n" ;
		}


		if ( currentlyrunningprocess.tempBurstTime == 0 ){
			//cout << "completion time: " << CPU << endl ;
			exitState(currentlyrunningprocess,0);
			cout << currentlyrunningprocess.name << " completed at : " << CPU << endl;
			//currentlyrunningprocess.completitionTime = CPU;
			readyState(currentlyrunningprocess,1 );
			
		}  
		
		currentlyrunningprocess.tempBurstTime -- ;
	

		if ( CPU % 5 == 0 && currentlyrunningprocess.tempBurstTime > 0  ){
			int r = generateRandom(0,1);
			if ( r == 0 ){
				// send to block queue and bring new process
				cout << currentlyrunningprocess.name << " blocked."<<endl ;
				blockState(currentlyrunningprocess,0);
				currentlyrunningprocess.clear();
				readyState(currentlyrunningprocess,1 ) ;
			}
			
		}

		
		CPU++ ;
		//cout << CPU << endl;
		sleep(1);
	}


}





