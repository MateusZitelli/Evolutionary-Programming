#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define POP_SIZE 5000
#define MAX_TREE_SIZE 200
#define INPUT_SIZE 10
#define SELECTEDS_PER_GEN 10
#define MUTATE_TAX 0.005
#define CROSS_TAX 0.4
#define DESIRED_MAX_SIZE 10

float INPUT[INPUT_SIZE];
float DESIRED_OUTPUT[INPUT_SIZE];

float MAX_ERROR;
/*Defined opps
  ITR | args | ID | CMT
  (+) -  2  -  0  |  -
  (-) -  2  -  1  |  -
  (*) -  2  -  2  |  -
  (/) -  2  -  3  |  -
  (sq)-  1  -  4  | sqrt
  (%) -  2  -  5  |  -
  (dt)-  0  -  6  | data
  (in)-  0  -  7  | inp
*/

int
get_type_arg_qnt (int t)
{
  switch (t)
    {
    case 0:
      return 2;
    case 1:
      return 2;
    case 2:
      return 2;
    case 3:
      return 2;
    case 4:
      return 1;
    case 5:
      return 2;
    case 6:
      return 0;
    case 7:
      return 0;
    }
}

#define OPERATIONS 6
#define DATA_RANGE 10

struct node
{
  int type;
  float data;
  struct tree * my_tree;
  struct node *children;
  struct node *next;
  /* struct node *parent; */
  int init;
};

struct tree
{
  struct node root;
  float input;
  int size;
  float fitness;
};

struct tree *trees_vector;

void init ();
void create_random_tree ();
float get_fitness ();
void set_tree_size ();
void mutate ();

void
print_tree2 (struct node *n)
{
  if(n == NULL) return;
  switch (n->type)
    {
    case 0:
      printf ("(+ ");
      break;
    case 1:
      printf ("(- ");
      break;
    case 2:
      printf ("(* ");
      break;
    case 3:
      printf ("(/ ");
      break;
    case 4:
      printf ("(sqrt ");
      break;
    case 5:
      printf ("(mod ");
      break;
    case 6:
      printf ("%0.3f", n->data);
      break;
    case 7:
      printf ("x");
      break;
    }
  int i;

  int args = get_type_arg_qnt (n->type) ;
  struct node * cur_node = n->children;
  for (i = 0; i < args - 2; i++)
    {
      print_tree2 (cur_node);
      printf (" ");
      cur_node = cur_node->next;
    }
  if (args > 0)
    print_tree2 (cur_node->next);

  switch (n->type)
    {
    case 0:
      printf (")");
      break;
    case 1:
      printf (")");
      break;
    case 2:
      printf (")");
      break;
    case 3:
      printf (")");
      break;
    case 4:
      printf (")");
      break;
    case 5:
      printf (")");
      break;
    }
}

void
print_tree (struct node *n)
{
  if(n == NULL) return;
  switch (n->type)
    {
    case 0:
      printf("(");
      print_tree(n->children);
      printf (" + ");
      print_tree(n->children->next);
      printf(")");
      break;
    case 1:
      printf("(");
      print_tree(n->children);
      printf (" - ");
      print_tree(n->children->next);
      printf(")");
      break;
    case 2: 
      printf("(");
      print_tree(n->children);
      printf (" * ");
      print_tree(n->children->next);
      printf(")");
      break;
    case 3:
      printf("(");
      print_tree(n->children);
      printf (" / ");
      print_tree(n->children->next);
      printf(")");
      break;
    case 4:
      printf (" sqrt(");
      print_tree(n->children);
      printf(") ");
      break;
    case 5:
      printf("(");
      print_tree(n->children);
      printf (" mod ");
      print_tree(n->children->next);
      printf(")");
      break;
    case 6:
      printf ("%0.3f", n->data);
      break;
    case 7:
      printf ("x");
      break;
    }
}

float
get_rand_data ()
{
  return (rand () % 10 / 10.0 - 0.5) * DATA_RANGE;
}

float
run (struct node *n)
{
  float arg, arg1;
  switch (n->type)
    {
    case 0:
      if(n->children->type == 6 && n->children->next->type == 6){
	n->type = 6;
	n->data = n->children->data + n->children->next->data;
	return n->data;
      }
      return run (n->children) + run (n->children->next);
    case 1:
      if(n->children->type == 6 && n->children->next->type == 6){
	n->type = 6;
	n->data = n->children->data - n->children->next->data;
	return n->data;
      }
      return run (n->children) - run (n->children->next);
    case 2:
      if(n->children->type == 6 && n->children->next->type == 6){
	n->type = 6;
	n->data = n->children->data * n->children->next->data;
	return n->data;
      }
      return run (n->children) * run (n->children->next);
    case 3:
      if(n->children->type == 6 && n->children->next->type == 6){
	n->type = 6;
	if(n->children->next->data == 0)
	  return 0;
	n->data = n->children->data / n->children->next->data;
	return n->data;
      }
      arg1 = run (n->children->next);
      if (arg1 == 0)
	return 0;
      return run (n->children) / arg1;
    case 4:
      if(n->children->type == 6){
	if(n->children->data < 0)
	  return 0;
	n->type = 6;
	n->data = sqrt(n->children->data);
      }
      arg = run (n->children);
      if (arg < 0)
	return 0;
      return sqrt (arg);
    case 5:
      return 0;
      if(n->children->type == 6 && n->children->next->type == 6){
	n->type = 6;
	if((int) n->children->next->data <= 0)
	  return 0;
	n->data = (int) n->children->data % (int) n->children->next->data;
	return n->data;
      }
      arg1 = (int) run (n->children->next);
      if ((int) arg1 <= 0)
	return 0;
      return (int) run (n->children) % (int) arg1;
    case 6:
      return n->data;
    case 7:
      return n->my_tree->input;
    }
}

void
set_tree_size (struct tree *t, struct node *an)
{
  ++t->size;
  if (an->children == NULL)
    return;
  int i;
  struct node * cur_node = an->children;
  for (i = 0; cur_node != NULL; i++)
    {
      set_tree_size (t, cur_node);
      cur_node = cur_node->next;
    }
}

float
run_tree (struct tree *t, float input)
{
  t->input = input;
  return run (&t->root);
}

float
get_fitness (struct tree *t)
{
  int i = 0;
  float difsum = 0;
  float diff;
  if(t->fitness != -1) return t->fitness;
  for (i = 0; i < INPUT_SIZE; i++)
    {
      diff = run_tree (t, INPUT[i]) - DESIRED_OUTPUT[i];
      difsum += sqrt(diff * diff);
    }
  difsum = (difsum / INPUT_SIZE + 1) * (t->size > 20? (t->size - 20) : 1 );
  t->fitness = difsum;
  return t->fitness;
}

void
init ()
{
  srand (time (0));
  int i;
  //Alloc the tress
  trees_vector = (struct tree *) malloc (sizeof (struct tree) * POP_SIZE);
  //Reset all tress
  for (i = 0; i < POP_SIZE; i++)
    {
      //Set atual tree as a new random tree
      create_random_tree (&trees_vector[i]);
      trees_vector[i].size = 0;
      set_tree_size (trees_vector + i, &trees_vector[i].root);
    }
}

void
recursive_random_generator (struct tree *t, struct node * node)
{
  int opp, i, arguments;
  if(rand() % 2){
    opp = rand() % 6;
  }else{
    node->data = get_rand_data();
    opp = rand() % 2 + 6;
  }
  node->type = opp;
  node->my_tree = t;
  arguments = get_type_arg_qnt(node->type);
  node->children = (struct node *) malloc(sizeof(struct node));
  struct node * cur_node = node->children;
  for (i = 0; i < arguments; cur_node = cur_node->next, i++)
    {
      cur_node = (struct node *) malloc(sizeof(struct node));
      recursive_random_generator (t, cur_node);
    }
}

void
create_random_tree (struct tree *t)
{
  recursive_random_generator (t, &t->root);
}

void
create_sorted (int n, struct tree *input_list, struct tree **sorted)
{
  int i, j, k;
  for (i = 0; i < n; i++)
    {
      sorted[i] = &input_list[i];
    }
  for (i = 0; i < POP_SIZE; i++)
    {
      for (j = 0; j < n; j++)
	{
	  if (get_fitness(sorted[j]) > get_fitness(&input_list[i]))
	    {
	      for (k = n - 1; k > j; k--)
		{
		  sorted[k] = sorted[k - 1];
		}
	      sorted[j] = &input_list[i];
	      break;
	    }
	}
    }
}

void
copy_sub_tree (struct node * source, struct node * destination, struct tree * destination_tree)
{
  destination->type = source->type;
  destination->data = source->data;
  destination->my_tree = destination_tree;
  struct node * cur_dest, * cur_source;
  for(cur_dest=destination->children, cur_source = source->children; cur_source != NULL; cur_dest=cur_dest->next, cur_source = cur_source->next){
    cur_dest = (struct node *) malloc(sizeof(struct node));
    copy_sub_tree(cur_source, cur_dest, destination_tree);
  }
}

void clear_tree(struct node * node){
  int i;
  struct node * n;
  for (n=node->children; n!=NULL; n=n->next)
    clear_tree(n);

  free(node->children);
  node->children = NULL;
  free(node);
}
void
copy_tree (struct tree *t, struct tree *receiver)
{
  clear_tree(&receiver->root);
  copy_sub_tree (&t->root, &receiver->root, t);
  receiver->fitness = t->fitness;
}

void get_node(struct node * sel, struct node * n, int num, int * an){
  int pos = 0;
  if(*an == num){
    *sel = *n;
    return;
  }
  ++*an;
  int i;
  struct node * cur_node;
  for(cur_node=n->children; cur_node == NULL;cur_node=cur_node->next){
    get_node(sel, cur_node, num, an);
  }
}



void mutate_tree(struct tree * t){
  int an = 0;
  int i;

  if(rand() % 100000 / 100000.0 < MUTATE_TAX){
    set_tree_size(t, &t->root);
    struct node * n_mut = (struct node *) malloc(sizeof(struct node *));
    get_node(n_mut, &t->root, rand() % t->size, &an);
    int before_args = get_type_arg_qnt(n_mut->type);
    if (n_mut->type == 6)
      {
	if (rand () % 2)
	  {
	    n_mut->type = rand () % 8;
	  }
	else
	  {
	    n_mut->data = get_rand_data ();
	  }
      }
    else
      {
	n_mut->type = rand () % 8;
      }
    int after_args = get_type_arg_qnt(n_mut->type);
    struct node * cur_node;
    if(after_args > before_args){
      for(cur_node = n_mut->children; cur_node != NULL; cur_node=cur_node->next);
      for(i = before_args; i < after_args; i++){
	recursive_random_generator(t, cur_node);
	cur_node=cur_node->next;
      }
    }else if(after_args < before_args){
      cur_node = n_mut->children;
      for(i = 0,cur_node = n_mut->children; i < before_args; i++,cur_node=cur_node->next);
      for(i = after_args; i < before_args; i++){
	clear_tree(cur_node);
	cur_node = cur_node->next;
      }
    }
  }
  set_tree_size(t, &t->root);
}

void cross_recursive(struct tree * t, struct tree * t2){
  int i;
  if(rand () % 100000 / 100000.0 < CROSS_TAX){
    int an = 0;
    struct node * n, * n2, * tmp;
    get_node(n2, &t2->root, rand() % t2->size , &an);
    an = 0;
    get_node(n, &t->root, rand() % t->size, &an);
    if(n == NULL || n2 == NULL) return;
    clear_tree(n);
    copy_node(n2, n, t2, t);
  }
}

void cross_tree(struct tree * t, struct tree * t2){
  t2->size = 0;
  t->size = 0;
  set_tree_size(t, &t->root);
  set_tree_size(t2, &t2->root);
  cross_recursive(t, t2);
  t->size = 0;
  set_tree_size(t, &t->root);
}

int
main ()
{
  init ();
  FILE *fp;
  fp=fopen("./data.csv", "w");
  int i, j, k;
  for (i = 0; i < INPUT_SIZE; i++){
    INPUT[i] = i;
  }
  DESIRED_OUTPUT[0] = 0;
  DESIRED_OUTPUT[1] = 1;
  for(i = 2; i < INPUT_SIZE; i++){
    DESIRED_OUTPUT[i] = DESIRED_OUTPUT[i - 1] + DESIRED_OUTPUT[i - 2];
  }
  MAX_ERROR = 0;
  for(i = 0; i < INPUT_SIZE; i++){
    MAX_ERROR += sqrt((INPUT[i] - DATA_RANGE / 2.0) *  (INPUT[i] - DATA_RANGE / 2.0));
  }
  MAX_ERROR /= INPUT_SIZE;
  struct tree *selected_trees =
    (struct tree *) malloc (sizeof (struct tree) * SELECTEDS_PER_GEN);
  struct tree **sorted =
    (struct tree **) malloc (sizeof (struct tree *) * 10);
  struct tree **roullete =
    (struct tree **) malloc (sizeof (struct tree *) * 50);
  for (k = 0; k < 300; k++)
    {
      for (i = 0; i < POP_SIZE; i++)
	{
	  trees_vector[i].fitness = -1;
	}
      int roullete_pos = 0;
      create_sorted (10, trees_vector, sorted);
      float fitness_sum = 0;
      for (i = 0; i < 10; i++)
	{
	  fitness_sum += sorted[i]->fitness;
	}
      for (i = 0; i < 10; i++)
	{
	  for (j = 0;
	       j < (sorted[i]->fitness) / fitness_sum * 50 && roullete_pos < 50;
	       j++)
	    {
	      roullete[roullete_pos++] = sorted[i];
	    }
	}
      struct tree *selected_trees =
	(struct tree *) malloc (sizeof (struct tree) * SELECTEDS_PER_GEN);
      roullete_pos = rand () % 50;
      printf ("Selecteds:\n");
      for (i = 0; i < SELECTEDS_PER_GEN; i++)
	{
	  copy_tree (roullete[roullete_pos], &selected_trees[i]);
	  roullete_pos = (roullete_pos + 50 / SELECTEDS_PER_GEN) % 50;
	  printf("####GEN CODE####\n");
	  print_tree(&selected_trees[i].root);
	  printf(" -> %f\n OUTPUT >> ", selected_trees[i].fitness);
	  for(j = 0; j < 10; j++){
	    printf("%f ", run_tree(&selected_trees[i], j));
	  }
	  printf("\n");
	}
      printf("################\n\n");
      for(i = 0; i < POP_SIZE; i++){
	copy_tree(&selected_trees[rand() % SELECTEDS_PER_GEN], &trees_vector[i]);
	mutate_tree(&trees_vector[i]);
	cross_tree(&trees_vector[i], &selected_trees[rand() % SELECTEDS_PER_GEN]);
	//print_tree(&trees_vector[i].root);
	//printf("\n");
      }
    }
  free(selected_trees);
  free(sorted);
  free(roullete);
  return 0;
}
