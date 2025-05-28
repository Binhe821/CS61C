#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_t *game, unsigned int snum);
static char next_square(game_t *game, unsigned int snum);
static void update_tail(game_t *game, unsigned int snum);
static void update_head(game_t *game, unsigned int snum);

/* Task 1 */
game_t *create_default_game() {
  // TODO: Implement this function.
  game_t* new_game = malloc(sizeof(game_t));
  new_game->num_rows = 18;
  new_game->board = malloc(new_game->num_rows*sizeof(char*));
  for(int i=0;i<new_game->num_rows;i++){
    new_game->board[i] = malloc(22*sizeof(char));
  }
  for(int i=0;i<=19;i++){
    new_game->board[0][i] = '#'; 
  }
  strcpy(new_game->board[17],new_game->board[0]);
  for(int i=0;i<=19;i++){
    if(i==0||i==19){
      new_game->board[1][i]='#';
    }
    else{
      new_game->board[1][i]=' ';
    }
  }
  for(int i=3;i<=16;i++){
    strcpy(new_game->board[i],new_game->board[1]);
  }
  for(int i=0;i<new_game->num_rows;i++){
    new_game->board[i][20] = '\n';
    new_game->board[i][21] = '\0';
  }
  new_game->num_snakes = 1;
  new_game->snakes = malloc(new_game->num_snakes*(sizeof(snake_t)));
  strcpy(new_game->board[2] , "# d>D    *         #\n");
  new_game->snakes[0].tail_row = 2;
  new_game->snakes[0].tail_col = 2;
  new_game->snakes[0].head_row = 2;
  new_game->snakes[0].head_col = 4;
  new_game->snakes[0].live = true;
  return new_game;
}
void free_game(game_t *game) {
  free(game->snakes);
  for(int i=0;i<game->num_rows;i++){
    free(game->board[i]);
  }
  free(game->board);
  free(game);
  return;
}

/* Task 3 */
void print_board(game_t *game, FILE *fp) {
  for(int i=0;i<game->num_rows;i++){
    for(int j=0;j<strlen(game->board[i]);j++){
      fprintf(fp,"%c",game->board[i][j]);
    }
  }
  return;
}

/*
  Saves the current game into filename. Does not modify the game object.
  (already implemented for you).
*/
void save_board(game_t *game, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(game, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_t *game, unsigned int row, unsigned int col) { return game->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch) {
  game->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  if(c=='s'||c=='w'||c=='a'||c=='d') return true;
  else return false;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  if(c=='S'||c=='W'||c=='A'||c=='D') return true;
  else return false;

}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  if(is_head(c)||is_tail(c))return true;
  if(c=='<'||c=='>'||c=='v'||c=='^')return true;
  return false;
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  if(c=='v')return 's';
  if(c=='^')return 'w';
  if(c=='<')return 'a';
  if(c=='>')return 'd';
  return '?';
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  if(c=='W')return '^';
  if(c=='S')return 'v';
  if(c=='A')return '<';
  if(c=='D')return '>';
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  if(c=='v'||c=='s'||c=='S')cur_row += 1;
  if(c=='^'||c=='w'||c=='W')cur_row -= 1;
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  if(c=='>'||c=='d'||c=='D')cur_col+=1;
  if(c=='<'||c=='a'||c=='A')cur_col-=1;
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_game. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_t *game, unsigned int snum) {
  unsigned int headcol = game->snakes[snum].head_col,headrow = game->snakes[snum].head_row;
  if(game->board[headrow][headcol]=='W')return game->board[headrow-1][headcol];
  if(game->board[headrow][headcol]=='S')return game->board[headrow+1][headcol];
  if(game->board[headrow][headcol]=='A')return game->board[headrow][headcol-1];
  if(game->board[headrow][headcol]=='D')return game->board[headrow][headcol+1];
  return '?';
}

/*
  Task 4.3

  Helper function for update_game. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_t *game, unsigned int snum) {
  unsigned int headcol = game->snakes[snum].head_col,headrow = game->snakes[snum].head_row;
  if(game->board[headrow][headcol]=='W'){
    game->board[headrow-1][headcol] = 'W';
    game->board[headrow][headcol] = '^';
    game->snakes[snum].head_row -= 1;
  }
  if(game->board[headrow][headcol]=='S'){
    game->board[headrow+1][headcol] = 'S';
    game->board[headrow][headcol] = 'v';
    game->snakes[snum].head_row += 1;
  }
  if(game->board[headrow][headcol]=='A'){
    game->board[headrow][headcol-1] = 'A';
    game->board[headrow][headcol] = '<';
    game->snakes[snum].head_col -= 1;
  }
  if(game->board[headrow][headcol]=='D'){
    game->board[headrow][headcol+1] = 'D';
    game->board[headrow][headcol] = '>';
    game->snakes[snum].head_col += 1;
  }
  
  return;
}

/*
  Task 4.4

  Helper function for update_game. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_t *game, unsigned int snum) {
unsigned int tailcol = game->snakes[snum].tail_col,tailrow = game->snakes[snum].tail_row;
  unsigned int newrow = get_next_row(tailrow,get_board_at(game,tailrow,tailcol));
  unsigned int newcol = get_next_col(tailcol,get_board_at(game,tailrow,tailcol)); 
  set_board_at(game,tailrow,tailcol,' ');
  set_board_at(game,newrow,newcol,body_to_tail(get_board_at(game,newrow,newcol))); 
  game->snakes[snum].tail_col = newcol;
  game->snakes[snum].tail_row=newrow;
  return;
}

/* Task 4.5 */
void update_game(game_t *game, int (*add_food)(game_t *game)) {
  for(unsigned int i=0;i<game->num_snakes;i++){
    char n = next_square(game,i);
    if(is_snake(n)==true||n=='#'){
       unsigned int headcol = game->snakes[i].head_col,headrow = game->snakes[i].head_row;
       set_board_at(game,headrow,headcol,'x');
       game->snakes[i].live=false;
    }
    else{
      update_head(game,i);
      if(n == '*') add_food(game);
      else update_tail(game,i);
    }
  }
  return;

}

/* Task 5.1 */
char *read_line(FILE *fp) {
  size_t cap = 128;
  size_t len=0;
  char* line = malloc(cap*sizeof(char));
  if(!line){
    return NULL;
  }
  while(fgets(line+len,(int)(cap-len),fp)){
  len = strlen(line);
  if(len > 0 && line[len-1] == '\n'){
    line[len]='\0';
    char* re = realloc(line,len+1);
    if(re)return re;
    else return line;
  }
  len = strlen(line);
  char* expand = realloc(line,cap*2);
  if(!expand){
    return line;
  }
  line = expand;
    cap*=2;
  }
  if(len==0){
    free(line);
    return NULL;
  }
  char* fin = realloc(line,len+1);
  return fin ? fin : line;
}

/* Task 5.2 */
game_t *load_board(FILE *fp) {
  game_t* news = malloc(sizeof(game_t));
  news->num_rows = 0;
  news->num_snakes = 0;
  news->board = NULL;
  news->snakes = NULL;
  char* lines= NULL;
  for(;;){
    lines = read_line(fp);
    if(!lines)break;
    news->num_rows++;
    char** temp = realloc(news->board,news->num_rows*sizeof(char*));
    if(!temp){
      for(int i=0;i<news->num_rows;i++){
        free(news->board[i]);
      }
      free(news->board);
      free(news);
      free(lines);
      return NULL;
    }
    news->board = temp;
    news->board[news->num_rows - 1] = malloc(sizeof(char)*(strlen(lines)+1));
    strcpy(news->board[news->num_rows-1],lines);
    news->board[news->num_rows - 1][strlen(lines)] = '\0';
    free(lines);
  }
  return news;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_t *game, unsigned int snum) {
  unsigned int nowcol = game->snakes[snum].tail_col;
  unsigned int nowrow = game->snakes[snum].tail_row;
  char now = get_board_at(game,nowrow,nowcol);
  while(true){
    unsigned int nextcol = get_next_col(nowcol,now);
    unsigned int nextrow = get_next_row(nowrow,now);
    now = get_board_at(game,nextrow,nextcol);
    if(is_head(now)){
      game->snakes[snum].head_row = nextrow;
      game->snakes[snum].head_col = nextcol;
      break;
    }
    nowcol = nextcol;
    nowrow = nextrow;
  }
  return;
}

/* Task 6.2 */
game_t *initialize_snakes(game_t *game) {
  game->num_snakes = 0;
  game->snakes = malloc(game->num_snakes*sizeof(snake_t));
  for(unsigned int i=0;i<game->num_rows;i++){
    for(unsigned int j=0;j<strlen(game->board[i]);j++){
      if(is_tail(game->board[i][j])){
	game->num_snakes ++;
        game->snakes = realloc(game->snakes,game->num_snakes*sizeof(snake_t));
	game->snakes[game->num_snakes-1].tail_col = j;
	game->snakes[game->num_snakes-1].tail_row = i;
	find_head(game,game->num_snakes-1);
      }
    }
  }	
  return game;
}
