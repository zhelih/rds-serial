/* wclique.c exact algorithm for finding one maximum-weight 

clique in an arbitrary graph,

10.2.2000, Patric R. J. Ostergard, 

patric.ostergard@hut.fi */



/* compile: gcc wclique.c -o wclique -O2 */



/* usage: wclique infile */



/* infile format: see http://www.tcs.hut.fi/~pat/wclique.html */



#include <stdio.h>

#include <sys/types.h>

#include <time.h>

#include <windows.h>

#include <map>

#include "List.h"



#define INT_SIZE (8*sizeof(int))

#define TRUE 1

#define FALSE 0

#define MAX_VERTEX 8000 /* maximum number of vertices */

#define MAX_WEIGHT 10 /* maximum weight of vertex */

#define is_edge(a,b) (bit[a][b/INT_SIZE]&(mask[b%INT_SIZE]))

#define is_2edge(a,b) (bit2[a][b/INT_SIZE]&(mask[b%INT_SIZE]))



int Vnbr,Enbr;          /* number of vertices/edges */

int clique[MAX_VERTEX]; /* table for pruning */

int bit[MAX_VERTEX][MAX_VERTEX/INT_SIZE+1];

int bit2[MAX_VERTEX][MAX_VERTEX/INT_SIZE+1]; /* 2 neughborhood */

int wt[MAX_VERTEX];



int pos[MAX_VERTEX];    /* reordering function */

int set[MAX_VERTEX];    /* current clique */

int rec[MAX_VERTEX];  /* best clique so far */

int saturated[MAX_VERTEX]; /* vertices ``saturated'' by degree */

int record;   /* weight of best clique */

int rec_level;          /* # of vertices in best clique */

int K = 5; /* value of k in k-plex*/



int** stack; /* our own stack to avoid system stack overflow */

int** plex_stack;   /* k-plex connectivity stack */

int** plex_connection; /* k-plex connectivity data */



unsigned mask[INT_SIZE];

void graph();           /* reads graph */

void convert_graph(linklist * &G0, long * &degree, long n);



time_t timer1;

double timer11;



int fileerror()

{

  printf("Error in graph file\n");

  exit(1);

}



//#ifdef DEBUG

long long int check2plex_cnt=0, sub_cnt=0, is2neigh_cnt=0;



int checkedge(int K, int level)

{

  int i,j,edge,co=0;

 edge=0;

 for (i=0; i<level; i++)

 {

   co=0;

   for(j=i+1;j<level;j++)

    {

       if (is_edge(set[i],set[j]))

       edge = (edge + 1);

// else

// {

//   co = co+1;

//   if(co>K)

//    { 

      //printf("value of co = %d \n", co);

//      return 0;

//     }

//   }

     }

}

 if (edge >= ((level*(level-1)/2) -K))

   return 1;

 else

   return 0;



}

int checkkplex(int K, int level)

{

  // returns: 

  //  0 - not success; 

  //  1 - success; 



  int i, j, degree;

    for (i=0; i<level; i++)

  {

    degree=0;

    int co=0;

    for (j=0; j<level; j++)

      {

        if (is_edge(set[i], set[j]))

        degree++;

    //  else

      //  {

          //co = co+1;

        //  if(co>=K)

        //    return 0;

    //  }

    }

          

    if (degree<level-K)

      return 0;

  }

  return 1;

}





int checkkplex2(int K, int level)

{

  // returns: 

  //  0 - not success; 

  //  1 - success; 



  int i, j, degree;

 

  i = level-1;

    degree=0;

    int co=0;

    for (j=0; j<level; j++)

      {

        if (is_edge(set[i], set[j]))

        degree++;

    }

          

    if (degree<level-K)

      return 0;

    else

  return 1;

}

int sub(int ct,int *table,int level,int weight,int l_weight)

{

  register int i,j,k,vi;

  int curr_weight,left_weight;

  int* newtable;

  int *p1,*p2;

  int saturated_cnt, fl;



  if(ct<=0) { /* 0 or 1 elements left; include these */

    if(ct==0) { 

      set[level++] = table[0];

      weight += l_weight;

    }

    if(weight>record) {

      record = weight;

      rec_level = level;

      for (i=0;i<level;i++) rec[i] = set[i];

    }

    return 0;

  }

  for(i=ct;i>=0;i--) {

    if((level==0)&&(i<ct)) return 0;

    k = table[i];

    if((level>0)&&(clique[k]<=(record-weight))) return 0;  /* prune */

    set[level] = k;

    curr_weight = weight+wt[k];

    l_weight -= wt[k];

    if(l_weight<=(record-curr_weight)) return 0; /* prune */

    saturated_cnt=0;

    for (vi=0; vi<level; vi++)

    if (is_edge(set[vi], k))

      plex_stack[level+1][set[vi]]=plex_stack[level][set[vi]];

    else

    {

      plex_stack[level+1][set[vi]]=plex_stack[level][set[vi]]+1;

      if (plex_stack[level+1][set[vi]]==K-1)

        saturated[saturated_cnt++]=set[vi];

    }

    plex_stack[level+1][k]=level?plex_stack[level][k]:0;

    if (plex_stack[level+1][k]>=K-1)

      saturated[saturated_cnt++]=k;





    newtable = stack[level];

    p1 = newtable;

    p2 = table;

    left_weight = 0;   

    while (p2<table+i) { 

      j = *p2++;

    //    if ( wt[j] && is_2edge(k, j) )

        {

        check2plex_cnt++;

        set[level+1]=j; // these 2 lines needed only for old k-plex verify procedure

      //  if (checkkplex(K, level+2)) // comment 4 lines below if uncommenting this

        /*The following lines for k-plex */

         fl = 1;

         for (vi=0; fl && (vi<saturated_cnt); vi++)

          fl=fl && is_edge(saturated[vi], j);

         if (fl && (is_edge(k, j) || plex_stack[level][j]<K-1))

           if(checkkplex2(K,level+2))



    //      if (checkedge(K-1, level+2)) /*use either one */

    //    if (checkedge(K, level+2)) 

        {

          *p1++ = j;

          left_weight += wt[j];

          plex_stack[level+1][j]=plex_stack[level][j]+(is_edge(k, j)?0:1);

        }

        }

      

    }

    if(left_weight<=(record-curr_weight)) continue;

    sub(p1-newtable-1,newtable,level+1,curr_weight,left_weight);

  }

  return 0;

}



vector<long> wclique(linklist * &G0, long * &degree, long n)

{

  time_t start_time = clock();

  int n1=0;

  for(long i=0;i<n;i++)

    if(degree[i] > 0)

      n1++;

  cout<<n1;

  if(n1>MAX_VERTEX)

  {

    cerr<<n<<" > MAX_VERTEX in wclique\n";

    vector<long> temp;

    return temp;

    //system("PAUSE");

    //exit(0);

  }

  int i,j,p;

  int min_wt,max_nwt,wth;

  int used[MAX_VERTEX];

  int nwt[MAX_VERTEX];

  int count;

  FILE *infile;



  /* initialize mask */

  mask[0] = 1;

  for(i=1;i<INT_SIZE;i++)

    mask[i] = mask[i-1]<<1;



  /* read graph */

  convert_graph(G0, degree, n);



  /* allocate our stack */

  stack=(int**)malloc(Vnbr*sizeof(int*));

  plex_stack=(int**)malloc(Vnbr*sizeof(int*));

  plex_connection=(int**)malloc(Vnbr*sizeof(int*));

  for (i=0; i<Vnbr; i++)

  {

    stack[i]=(int*)malloc(Vnbr*sizeof(int));

    plex_stack[i]=(int*)malloc(Vnbr*sizeof(int));

    plex_connection[i]=(int*)malloc((K-1)*sizeof(int));

    plex_stack[0][i]=0;

  }

  /* "start clock" */

  timer1 = time(NULL);



  /* order vertices */

  for(i=0;i<Vnbr;i++) {

    nwt[i] = 0;

    for(j=0;j<Vnbr;j++)

      if (is_edge(i,j)) nwt[i] += wt[j];

  }

  for(i=0;i<Vnbr;i++)

    used[i] = FALSE;

  count = 0;

  do {

    min_wt = MAX_WEIGHT+1; max_nwt = -1; 

    for(i=Vnbr-1;i>=0;i--)

      if((!used[i])&&(wt[i]<min_wt))

        min_wt = wt[i];

    for(i=Vnbr-1;i>=0;i--) {

      if(used[i]||(wt[i]>min_wt)) continue;

      if(nwt[i]>max_nwt) {

        max_nwt = nwt[i];

        p = i;

      }

    }

    pos[count++] = p;

    used[p] = TRUE;

    for(j=0;j<Vnbr;j++)

      if((!used[j])&&(j!=p)&&(is_edge(p,j)))

        nwt[j] -= wt[p];

  } while(count<Vnbr);

  

  /* main routine */

  record = 0;

  wth = 0;

  for(i=0;i<Vnbr;i++) {

    wth += wt[pos[i]];

    sub(i,pos,0,0,wth);

    clique[pos[i]] = record;

    timer11 = time(NULL)-timer1;

    fprintf(stderr, "level = %3d(%d) best = %2d(%2d) time = %8.2f\n",i+1,Vnbr,rec_level,record,timer11);

    fflush(stderr);

  }

  vector<long> cliq;

  for(i=0;i<rec_level;i++) 

    cliq.push_back(rec[i]);

  printf("Record: %d %d\n", rec_level, record);

  for(i=0;i<rec_level;i++) 

    printf ("%d %d\n",rec[i], wt[rec[i]]);

  printf ("\n");

  cerr<<"Wclique Size = "<<Vnbr<<" nodes.\n";

  cerr<<"Wclique Time = "<<(double)(clock()-start_time)/CLOCKS_PER_SEC<<endl;

  return cliq;

}



void convert_graph(linklist * &G0, long * &degree_table, long n)

{

  register long i,j;

  int weight,degree,entry;

  long id;

  map<long, long> node_id;

  pair<long, long> ni;

  Vnbr=0;

  for(long i=0;i<n;i++)

  {

    if(degree_table[i] > 0)

    {     

      ni.first=i;

      ni.second=Vnbr;

      node_id.insert(ni);

      Vnbr++;

    }

  }



  for(i=0;i<Vnbr;i++)     /* empty graph table */

    for(j=0;j<Vnbr/INT_SIZE+1;j++)

      bit[i][j] = 0;

  register int k=0;

  for(i=0;i<n;i++) {    

    if(degree_table[i]<=0)  

      continue;

    wt[k] = 1;

    for(j=0;j<degree_table[i];j++) {

      if(j==0) id=G0[i].first();

      else id=G0[i].next();

      entry = node_id.find(id)->second;

      bit[k][entry/INT_SIZE] |= mask[entry%INT_SIZE]; /* record edge */

    }

    k++;

  }

}



//void graph(fp) 

//FILE *fp;

//{

//  register int i,j;

//  int weight,degree,entry;

//

//  if(!fscanf(fp,"%d %d\n",&Vnbr,&Enbr))

//    fileerror(); 

//  for(i=0;i<Vnbr;i++)     /* empty graph table */

//    for(j=0;j<Vnbr/INT_SIZE+1;j++)

//      bit[i][j] = 0;

//  for(i=0;i<Vnbr;i++) {

//    if(!fscanf(fp,"%d %d",&weight,&degree))

//      fileerror(); 

//    wt[i] = weight;

//    for(j=0;j<degree;j++) {

//      if(!fscanf(fp,"%d",&entry))

//        fileerror();

//      bit[i][entry/INT_SIZE] |= mask[entry%INT_SIZE]; /* record edge */

//    }

//  }

//  fclose(fp);

//}

//
