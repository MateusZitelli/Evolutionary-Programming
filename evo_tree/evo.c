#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define POP_SIZE 10000
#define MAX_TREE_SIZE 200
#define INPUT_SIZE 26
#define SELECTEDS_PER_GEN 10
#define MUTATE_TAX 0.01
#define CROSS_TAX 0.1

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
  float *input;
  struct node *sons;
  struct node *dad;
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
print_tree (struct node *n)
{
  if (n == NULL)
    return;
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
      printf ("%f", n->data);
      break;
    case 7:
      printf ("X");
      break;
    }
  int i;
  int size = get_type_arg_qnt (n->type);
  for (i = 0; i < size - 1; i++)
    {
      print_tree (&n->sons[i]);
      printf (" ");
    }
  if (size > 0)
    print_tree (&n->sons[size - 1]);
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

float
get_rand_data ()
{
  return rand () % 10 / 10.0 * DATA_RANGE;
}

float
run (struct node *n)
{
  float arg, arg1;
  switch (n->type)
    {
    case 0:
      if(n->sons[0].type == 6 && n->sons[1].type == 6){
	n->type = 6;
	n->data = n->sons[0].data + n->sons[1].data;
	return n->data;
      }
      return run (&n->sons[0]) + run (&n->sons[1]);
    case 1:
      if(n->sons[0].type == 6 && n->sons[1].type == 6){
	n->type = 6;
	n->data = n->sons[0].data - n->sons[1].data;
	return n->data;
      }
      return run (&n->sons[0]) - run (&n->sons[1]);
    case 2:
      if(n->sons[0].type == 6 && n->sons[1].type == 6){
	n->type = 6;
	n->data = n->sons[0].data * n->sons[1].data;
	return n->data;
      }
      return run (&n->sons[0]) * run (&n->sons[1]);
    case 3:
      if(n->sons[0].type == 6 && n->sons[1].type == 6){
	n->type = 6;
	if(n->sons[1].data == 0)
	  return 0;
	n->data = n->sons[0].data / n->sons[1].data;
	return n->data;
      }
      arg1 = run (&n->sons[1]);
      if (arg1 == 0)
	return 0;
      return run (&n->sons[0]) / arg1;
    case 4:
      if(n->sons[0].type == 6){
	if(n->sons[0].data < 0)
	  return 0;
	n->type = 6;
	n->data = sqrt(n->sons[0].data);
      }
      arg = run (&n->sons[0]);
      if (arg < 0)
	return 0;
      return sqrt (arg);
    case 5:
      if(n->sons[0].type == 6 && n->sons[1].type == 6){
	n->type = 6;
	if((int) n->sons[1].data == 0)
	  return 0;
	n->data = (int) n->sons[0].data % (int) n->sons[1].data;
	return n->data;
      }
      arg1 = (int) run (&n->sons[1]);
      if ((int) arg1 == 0)
	return 0;
      return (int) run (&n->sons[0]) % (int) arg1;
    case 6:
      return n->data;
    case 7:
      return *n->input;
    }
}

void
set_tree_size (struct tree *t, struct node *an)
{
  ++t->size;
  if (an == NULL)
    return;
  int i;
  for (i = 0; i < get_type_arg_qnt (an->type); i++)
    {
      set_tree_size (t, &an->sons[i]);
    }
}

float
run_tree (struct tree *t, float input)
{
  t->input = input;
  return run (&t->root);
}

float
get_fitness (struct tree *t, float input[], float desired_output[])
{
  int i = 0;
  float difsum = 0;
  for (i = 0; i < INPUT_SIZE; i++)
    {
      float diff = run_tree (t, input[i]) - desired_output[i];
      //printf("%f\n", diff);
      diff = diff * diff;
      if (diff > 50 * 50)
	diff = 50 * 50;
      difsum += diff;
    }
  difsum = 50 * INPUT_SIZE - sqrt (difsum);
  t->size = 0;
  set_tree_size(t, &t->root);
  float size_corrector = t->size < 10? 1 : 10.0 / (t->size);
  t->fitness = difsum * size_corrector / (50 * INPUT_SIZE);
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
recursive_random_generator (struct tree *t, struct node *actual_node,
			    int node_type, struct node *dad)
{
  //node_type = -1 - first node | 0 - opp | 1 - data | 2 - input
  if(rand() % 2){
    set_node(actual_node, rand() % 6, dad, t);
  }else{
    actual_node->data = get_rand_data();
    set_node(actual_node, rand() % 2 + 6, dad, t);
  }
}

int
set_node (struct node *n, int opp, struct node *dad, struct tree *t)
{
  int i;
  n->type = opp;
  n->dad = dad;
  n->input = &t->input;
  int arguments = get_type_arg_qnt(n->type);
  if(arguments == 0) return 0;
  n->sons = (struct node *) malloc (sizeof (struct node) * arguments);
  n->init = 1;
  for (i = 0; i < arguments; i++)
    {
      recursive_random_generator (t, &n->sons[i], rand () % 3, n);
    }
  return 0;
}

void
create_random_tree (struct tree *t)
{
  recursive_random_generator (t, &t->root, -1, NULL);
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
	  if (sorted[j]->fitness < input_list[i].fitness)
	    {
	      for (k = n - 1; k > j + 1; k--)
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
copy_node (struct node *n, struct node *r, struct tree *t, struct tree *tr)
{
  r->sons = (struct node *) malloc(get_type_arg_qnt(n->type) * sizeof(struct node));
  
}

void clear_tree(struct node * node){
  int i;
  if(get_type_arg_qnt(node->type) == 0 || node->sons == NULL) return;
  for(i = 0; i < get_type_arg_qnt(node->type);i++){
    clear_tree(&node->sons[i]);
  }
  free(node->sons);
  node->sons = NULL;
}
void
copy_tree (struct tree *t, struct tree *receiver)
{
  clear_tree(&receiver->root);
  copy_node (&t->root, &receiver->root, t, receiver);
  receiver->fitness = t->fitness;
}


void
mutate_recursive (struct node *n, struct tree *t)
{
  int last_args = get_type_arg_qnt (n->type);
  int i;
  if (rand () % 100000 / 100000.0 < MUTATE_TAX)
    {
      //Case node represents datum, the datum or the function can be mutated
      if (n->type == 6)
	{
	  if (rand () % 2)
	    {
	      n->type = rand () % 8;
	    }
	  else
	    {
	      n->data = get_rand_data ();
	    }
	}
      else
	{
	  n->type = rand () % 8;
	}
    }
  if(n->sons != NULL)
    for (i = 0; i < last_args; i++)
      mutate_recursive (&n->sons[i], t);
  int args = get_type_arg_qnt (n->type);
  if (args > last_args)
    {
      struct node *tmp =
	(struct node *) malloc (sizeof (struct node) * last_args);
      for (i = 0; i < last_args; i++)
	{
	  tmp[i] = n->sons[i];
	}
      free (n->sons);
      n->sons = NULL;
      n->sons = (struct node *) malloc (sizeof (struct node) * args);
      for (i = 0; i < last_args; i++)
	{
	  n->sons[i] = tmp[i];
	}
      free (tmp);

      for (i = last_args; i < args; i++)
	{
	  set_node (&n->sons[i], rand () % 8, n, t);
	}
    }

}

void
mutate_tree (struct tree *t)
{
  mutate_recursive (&t->root, t);
}

struct node * get_node(struct node * n, int num, int * an){
  if(++(*an) == num){
    return n;
  }else{
    int i;
    for(i = 0; i < get_type_arg_qnt(n->type);i++){
      return get_node(&n->sons[i], num, an);
    }
  }
}

void cross_recursive(struct node * n, struct tree * t, struct tree * t2){
  int i;
  if(rand () % 100000 / 100000.0 < CROSS_TAX){
    int an = 0;
    set_tree_size(t2, &t2->root);
    struct node * n2 = get_node(&t2->root, rand() % t2->size, &an);
    //copy_node(n2, n, t2, t);
  }
  for(i = 0; i < get_type_arg_qnt(n->type); i++){
    cross_recursive(&n->sons[i], t, t2);
  }
}

void cross_tree(struct tree * t, struct tree * t2){
  cross_recursive(&t->root, t, t2);
}

int
main ()
{
  init ();
  int i, j, k;
  float input[INPUT_SIZE];
  for (i = 0; i < INPUT_SIZE; i++)
    input[i] = i;
  float output[INPUT_SIZE] =  {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101};    
  for (k = 0; k < 3000; k++)
    {
      for (i = 0; i < POP_SIZE; i++)
	{
	  get_fitness (&trees_vector[i], input, output);
	}
      struct tree **roullete =
	(struct tree **) malloc (sizeof (struct tree *) * 50);
      int roullete_pos = 0;
      int n = 10;
      struct tree **sorted =
	(struct tree **) malloc (sizeof (struct tree *) * n);
      create_sorted (n, trees_vector, sorted);
      float fitness_sum = 0;
      for (i = 0; i < n; i++)
	{
	  fitness_sum += sorted[i]->fitness;
	}
      for (i = 0; i < n; i++)
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
      for(i = 0; i < SELECTEDS_PER_GEN; i++) selected_trees[i].root.sons = NULL;
      roullete_pos = rand () % 50;
      printf ("Selecteds:\n");
      for (i = 0; i < SELECTEDS_PER_GEN; i++)
	{
	  copy_tree (roullete[roullete_pos], &selected_trees[i]);
	  roullete_pos = (roullete_pos + 50 / SELECTEDS_PER_GEN) % 50;
	  print_tree(&selected_trees[i].root);
	  printf(" -> %f", trees_vector[i].fitness);
	  printf("\n");
	}
      for(i = 0; i < POP_SIZE; i++){
	//copy_tree(&selected_trees[rand() % SELECTEDS_PER_GEN], &trees_vector[i]);
	mutate_tree(&trees_vector[i]);
	cross_tree(&trees_vector[i], &selected_trees[rand() % SELECTEDS_PER_GEN]);
	//print_tree(&trees_vector[i].root);
	//printf("\n");
      }
      //clear_tree(&selected_trees[0].root);
      free(selected_trees);
      free(sorted);
      free(roullete);
      printf("\n\n");
    }
  return 0;
}
