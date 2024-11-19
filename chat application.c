#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_NAME_LEN 50
#define MAX_MESSAGE_LEN 256

//linked list of messages
typedef struct Message {
    int sender_id;
    char text[MAX_MESSAGE_LEN];
    struct Message* next;
} Message;
typedef struct User {
    int id;
    char name[MAX_NAME_LEN];
    Message* messages;
} User;
typedef struct Node {
    int user_id;
    struct Node* next;
} Node;
typedef struct Graph {
    int numUsers;
    User* users;
    Node** adjacencyList;
} Graph;

//creation of graph for no of users
Graph* create_graph(int max_users) {
    int i;
    Graph* g = (Graph*)malloc(sizeof(Graph));
    g->numUsers = 0;
    g->users = (User*)malloc(max_users * sizeof(User));
    g->adjacencyList = (Node**)malloc(max_users * sizeof(Node*));

    for (i = 0; i < max_users; i++) {
        g->users[i].id = -1;  
        g->users[i].messages = NULL; 
        g->adjacencyList[i] = NULL;
    }
    return g;
}
//adding users to graph
void add_user(Graph* g, const char* username,int max_users) {
    if (g->numUsers >= max_users) {
        printf("Cannot add more users.\n");
        return;
    }
    int id = g->numUsers;  
    g->users[id].id = id; 
    
    strcpy(g->users[id].name, username);
    g->numUsers++;
    printf("User %s added with ID %d.\n", username, id);
}//creating a adjacency list through friend connection
void add_friend(Graph* g, int user_id1, int user_id2) {
    if (user_id1 < 0 || user_id2 < 0 || user_id1 >= g->numUsers || user_id2 >= g->numUsers) {
        printf("Invalid user IDs.\n");
        return;
    }
    Node* newNode1 = (Node*)malloc(sizeof(Node));
    newNode1->user_id = user_id2;
    newNode1->next = g->adjacencyList[user_id1];
    g->adjacencyList[user_id1] = newNode1;

    Node* newNode2 = (Node*)malloc(sizeof(Node));
    newNode2->user_id = user_id1;
    newNode2->next = g->adjacencyList[user_id2];
    g->adjacencyList[user_id2] = newNode2;

    printf("User %d and User %d are now friends.\n", user_id1, user_id2);
}
void send_message(Graph* g, int sender_id, int receiver_id, const char* message) {
    if (sender_id < 0 || sender_id >= g->numUsers || receiver_id < 0 || receiver_id >= g->numUsers) {
        printf("Invalid user IDs.\n");
        return;
    }
    Message* newMessage = (Message*)malloc(sizeof(Message));
    strcpy(newMessage->text, message);
    newMessage->sender_id = sender_id;
    newMessage->next = g->users[receiver_id].messages;
    g->users[receiver_id].messages = newMessage;
    char filename[50];
    sprintf(filename, "chat_%d_%d.txt", sender_id, receiver_id);
    FILE* file = fopen(filename, "a");
    if (file == NULL) {
        perror("Failed to open chat file");
        return;
    }
    fprintf(file, "From User %d: %s\n", sender_id, message);
    fclose(file);

    printf("Message sent from User %d to User %d: %s\n", sender_id, receiver_id, message);
}

void display_messages(Graph* g, int user_id) {
    if (user_id < 0 || user_id >= g->numUsers) {
        printf("Invalid user ID.\n");
        return;
    }
    Message* current = g->users[user_id].messages;
    printf("Messages for User %d (%s):\n", user_id, g->users[user_id].name);
    while (current != NULL) {
        printf("From User %d: %s\n", current->sender_id, current->text);
        current = current->next;
    }
}

void dfs(Graph* g, int user_id, int* visited) {
    visited[user_id] = 1;
    printf("User %d ", user_id);
    Node* temp = g->adjacencyList[user_id];
    while (temp != NULL) {
        if (!visited[temp->user_id]) {
            dfs(g, temp->user_id, visited);
        }
        temp = temp->next;
    }
}

void find_friend_groups(Graph* g) {
    int visited[MAX_USERS] = {0};
    printf("Friend groups:\n");
    int i;
    for (i = 0; i < g->numUsers; i++) {
        if (!visited[i]) {
            printf("Group: ");
            dfs(g, i, visited);
            printf("\n");
        }
    }
}

void free_graph(Graph* g) {
	int i;
    for (i = 0; i < g->numUsers; i++) {
        Node* temp = g->adjacencyList[i];
        while (temp != NULL) {
            Node* toDelete = temp;
            temp = temp->next;
            free(toDelete);
        }
        Message* current = g->users[i].messages;
        while (current != NULL) {
            Message* toDelete = current;
            current = current->next;
            free(toDelete);
        }
    }
    free(g->adjacencyList);
    free(g->users);
    free(g);
    printf("Graph memory freed.\n");
}
int main() {
    int max_users;
    printf("Enter maximum number of users: ");
    scanf("%d", &max_users);
    Graph* g = create_graph(max_users);
    int choice, user_id1, user_id2;
    char username[MAX_NAME_LEN], message[MAX_MESSAGE_LEN];
    while (1) {
        printf("\n=== Chat Application Menu ===\n");
        printf("1. Add user\n");
        printf("2. Add friend\n");
        printf("3. Send Message\n");
        printf("4. Display Messages for a User\n");
        printf("5. Find Friend Groups\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1:
                printf("Enter username: ");
                scanf("%s", username);
                add_user(g, username,max_users);
                break;
            case 2:
                printf("Enter two user IDs to add as friends: ");
                scanf("%d %d", &user_id1, &user_id2);
                add_friend(g, user_id1, user_id2);
                break;
            case 3:
                printf("Enter sender ID, receiver ID, and message: ");
                scanf("%d %d", &user_id1, &user_id2);
                getchar();  
                fgets(message, MAX_MESSAGE_LEN, stdin);
                message[strcspn(message, "\n")] = '\0';  
                send_message(g, user_id1, user_id2, message);
                break;
            case 4:
                printf("Enter user ID to display messages for: ");
                scanf("%d", &user_id1);
                display_messages(g, user_id1);
                break;
            case 5:
                find_friend_groups(g);
                break;
            case 6:
                free_graph(g);
                exit(0);
            default:
                printf("Invalid choice. Try again.\n");
        }
    }
}

