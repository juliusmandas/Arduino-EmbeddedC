/* Queue */

struct Node {
  Frame *data;
  struct Node* next;
};

struct Node* front = NULL;
struct Node* back = NULL;

void queue(struct Frame *frame)
{
  struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
  newNode->data = frame;
  newNode->next = NULL;

  if (front == NULL && back == NULL)
  {
    front = newNode;
    back = newNode;
    return;
  }

  //back->next = newNode;
  back->next = newNode;
  back = newNode;
}

void dequeue()
{
  if (front == NULL)
    return;

  struct Node* deadNode = front;
  if (front == back)
  {
    front = NULL;
    back = NULL;

  }
  else
  {
    front = front->next;
  }
  //TODO: null check
  //one free / malloc
  free(deadNode->data);
  free(deadNode);
}

int isEmpty()
{
  return front == NULL;
}

int hasNext()
{
  return (front != NULL && front->next != NULL);
}

//Call isEmpty() first!
//change to void and pointer argument
struct Frame* getFront()
{

  if (front == NULL)
    return;
  return front->data;
}


/* End Queue*/
