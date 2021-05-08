#include <iostream>
#include <fstream>
#include <sys/shm.h>
#include <pthread.h>

using namespace std;

int** extendArray(int** arr, int &size){

int newSize=size*2;

int** newArr=new int*[newSize];

for(int i=0;i<newSize;i++){
newArr[i]=new int[2];
}

for(int i=0;i<size;i++){
newArr[i]=arr[i];
}

for(int i=0;i<size;i++){
delete[] arr[i];
}

delete[] arr;

size=newSize;

return newArr;
}


void* add(void* param){


int key=shmget(12327, 1024, 0 );

int* ptr= (int*) shmat(key, NULL, 0); 


int* arr=(int*) param;

//cout<<"In Function Add\n";
//cout<<arr[0]<<","<<arr[1]<<"\n";

//int id=pthread_self();

//cout<<"Thread ID: "<<id<<endl;
//cout<<"Count: "<<ptr[0]<<endl;

//Third index in arr has the location where resultant number should be saved

ptr[arr[2]]=arr[0]+arr[1];

//cout<<"Result: "<<pthread_self()<<endl;//<<": "<<arr[0]+arr[1]<<endl;

shmdt(ptr);

pthread_exit(0);
}

int main(){

ifstream fin;

fin.open("input.txt");

//Initially we assumed that array can't be greater than 40. If there are more than 40 rows array will double its size 
int size=40;

int count=0;

int** data=new int*[size];

for(int i=0;i<size;i++){
data[i]=new int[3];
}

int key=shmget(12327, 1024, IPC_CREAT | IPC_EXCL | 0666); 

int* ptr= (int*) shmat(key, NULL, 0); 

if(fin.is_open()){

int a;

while(!fin.eof()){
if(count<size){
fin>>a;

//Value of first array
data[count][0]=a;

fin>>a;

if(fin.eof()){
break;
}

//Value of second array
data[count][1]=a;

//Third index has index of shared memory where data should be stored 
data[count][2]=count;

count++;
}
else {
data=extendArray(data,size);
}
}
fin.close();
}
else {
cout<<"input.txt can't be opened\n\n";
}

//Array of thread IDs equal to number of rows in input file
pthread_t* threadID=new pthread_t[count];

for(int i=0;i<count;i++){

if(pthread_create(&threadID[i],NULL,&add,(void*)data[i])==-1){
cout<<"Thread-"<<i<<" Creation Failed\n\n";
return 1;
}
}

for(int i=0;i<count;i++){

if(pthread_join(threadID[i],NULL)==-1){
cout<<"Thread-"<<i<<" Join Failed\n\n";
return 1;
}
}

ofstream fout;

fout.open("result.txt");

if(fout.is_open()){

for(int i=0;i<count;i++){
fout<<ptr[i]<<"\n";
}

fout.close();
} else {

cout<<"result.txt could not be opened\n";
return 1;

}

for(int i=0;i<size;i++){
delete[] data[i];
}

delete[] data;

shmdt(ptr);
 
shmctl(key, IPC_RMID, NULL);

}
