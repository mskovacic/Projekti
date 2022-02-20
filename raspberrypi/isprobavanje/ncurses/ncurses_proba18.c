#include <stdlib.h>
#include <curses.h>
#include <menu.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

/*


     REQ_LEFT_ITEM         Move left to an item.
     REQ_RIGHT_ITEM      Move right to an item.
     REQ_UP_ITEM         Move up to an item.
     REQ_DOWN_ITEM       Move down to an item.
     REQ_SCR_ULINE       Scroll up a line.
     REQ_SCR_DLINE          Scroll down a line.
     REQ_SCR_DPAGE          Scroll down a page.
     REQ_SCR_UPAGE         Scroll up a page.
     REQ_FIRST_ITEM     Move to the first item.
     REQ_LAST_ITEM         Move to the last item.
     REQ_NEXT_ITEM         Move to the next item.
     REQ_PREV_ITEM         Move to the previous item. 
     REQ_TOGGLE_ITEM     Select/deselect an item.
     REQ_CLEAR_PATTERN     Clear the menu pattern buffer.
     REQ_BACK_PATTERN      Delete the previous character from the pattern buffer.
     REQ_NEXT_MATCH     Move to the next item matching the pattern match.
     REQ_PREV_MATCH     Move to the previous item matching the pattern match.

*/
char *choices[] = {
                        "Choice 1",
                        "Choice 2",
                        "Choice 3",
                        "Choice 4",
                        "Exit",
                  };

int main()
{	ITEM **my_items;
	int c;				
	MENU *my_menu;
	int n_choices, i;
	ITEM *cur_item;
	
	
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	
	n_choices = ARRAY_SIZE(choices);
	my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));

	for(i = 0; i < n_choices; ++i)
		my_items[i] = new_item(choices[i], choices[i]);
	my_items[n_choices] = (ITEM *)NULL;

	my_menu = new_menu((ITEM **)my_items);
	post_menu(my_menu);
	mvprintw(LINES - 2, 0, "F1 to Exit");
	refresh();
	mvprintw(LINES - 3, 0, "Upisi znak:");
	c=getch();
	mvprintw(LINES-4,0,"upisan %c",c);

	while((c = getch()) != KEY_F(1))
	{   switch(c)
	    {	case KEY_DOWN:
		        menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
		}
	}	

	free_item(my_items[0]);
	free_item(my_items[1]);
	free_menu(my_menu);
	endwin();
}
