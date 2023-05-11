#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "calendar.h"

/*This function intializes a calendar struct based on the given parameters,
and it allocates memory for the calendar struct, the name, and events. Returns
failure if parameters are null or invalid, or if allocation fails*/
int init_calendar(const char *name, int days, 
		    int (*comp_func) (const void *ptr1, const void *ptr2),
		    void (*free_info_func) (void *ptr), Calendar ** calendar){

  Calendar *initialize = malloc(sizeof(Calendar));

  /*Checks if any parameters are invalid or if memory allocation fails*/
  if(!calendar || !name || days < 1 || !initialize) {
      return FAILURE;
    }

  /*allocates memory for name and copies name parameter*/
  initialize->name = malloc(strlen(name)+1);
  strcpy(initialize->name, name);

  /*allocates memory for event and sets days to parameter, total_events to 0*/
  initialize->events = calloc(days, sizeof(Event));
  initialize->days = days;
  initialize->total_events = 0;

  /*Sets function pointers*/
  initialize->comp_func = comp_func;
  initialize->free_info_func = free_info_func;

  (*calendar) = initialize;
  return SUCCESS;
}

/*This function prints the calendar's name, days and total number of
events if the last parameter is true, and it always prints the name, start
time, and duration of each event. It prints to a designated output stream*/
int print_calendar(Calendar *calendar, FILE *output, int print_fields){
  
  Event *curr;
  int i; 
  
  /*Null Checks*/
  if(!calendar || !output){
    return FAILURE;
  }
  
  /*Print calendar fields if third parameter is true*/
  if(print_fields){

    fprintf(output, "Calendar's Name: \"%s\"\n", calendar->name);
    fprintf(output, "Days: %d\nTotal Events: %d\n\n",
	    calendar->days, calendar->total_events);
  }

  /*Prints name, start time, and duration of all events*/
  printf("**** Events ****\n");
  
  if(calendar->total_events > 0){
    for(i = 0; i<calendar->days; i++){

      fprintf(output, "Day %d\n", i+1);
      curr = calendar->events[i];

      while(curr){
	/*prints name, start time, and duration.*/

	fprintf(output, "Event's Name: \"%s\"", curr->name);
	fprintf(output, ", Start_time: %d, Duration: %d\n",
	       curr->start_time, curr->duration_minutes);
	curr = curr->next;
      }

    }
  }
return SUCCESS;
}

/*This function adds a specified event to the specified event list
  in ascending order, it returns false if parameters are null or invalid,
  if the event already exists, and if allocation fails*/
int add_event(Calendar *calendar, const char *name, int start_time,
	   int duration_minutes, void *info, int day) {

  Event *curr, *find, *add, *prev = NULL;

  /*Null checks and validates given start_time and duration*/
  if(!calendar || !name || start_time < 0 || start_time > 2400
     || duration_minutes <= 0){

    return FAILURE;
  }

  /*Returns failure if event already exists*/
  if(find_event(calendar, name, &find) == SUCCESS){
    
    return FAILURE;
  }
  else{

    /*allocates space for new node and name, returns FAILURE if either
      allocation fails. Also sets the new nodes start_time, duration,
    and info to the given parameters*/
    add = malloc(sizeof(Event));	       
    if(add){
      
      add->name = malloc(strlen(name)+1);
    
      if(add->name){
	
	strcpy(add->name, name);
	add->start_time = start_time;
	add->duration_minutes = duration_minutes;
	add->info = info;
	add->next = NULL;
       
      }      
      else{
	return FAILURE;
      }
    }
    else{
      return FAILURE;
    }
  }
  

  /*Makes the new node the head if the head is null*/
  if(!calendar->events[day-1]){

    calendar->events[day-1] = add;
  }
  else{
    curr = calendar->events[day-1];
    /*Searches for the right spot to insert the event*/
    while(curr && calendar->comp_func(curr, add) < 0){
      prev = curr;
      curr = curr->next;
    }
    
    if(!curr){;
      prev->next = add;
    }
    else {
      if(!prev){
	add->next = calendar->events[day-1];
	calendar->events[day-1] = add;

      }
      else{
	add->next = curr;
	prev->next = add;
      }

    }
  }
  
  calendar->total_events++;
  return SUCCESS;
  }

/*This function returns a pointer to a specified event via the out
  parameter, it returns FAILURE if the specified does not exist*/
int find_event(Calendar *calendar, const char *name, Event **event) {

  Event *curr;
  int i;

  /* Null check on first two parameters*/
  if (!calendar || !name) {
      return FAILURE;
    }


  for (i = 0; i < calendar->days; i++) {
    curr = calendar->events[i];

    while(curr){
      /*Compares current name to parameter name, if they match,
	the out parameter is set to the current event and SUCCESS is returned*/
      if (!strcmp(curr->name, name)) {
	
	/*Only sets event to curr if event!=NULL*/
	if(event){ 
	  (*event) = curr;
	}

	return SUCCESS;
      }

      /* Checks next event */
      curr = curr->next;
    }    
  }

  /*If the event was never found, the function will return failure*/
  return FAILURE;

}

/*This function searches for a specified event in a specific day, and returns
a pointer to that event via the out parameter. It returns FAILURE if parameters
are null or invalid or the event does not exist*/
int find_event_in_day(Calendar * calendar, const char *name, int day,
		       Event ** event){

  Event *curr;
  /* Null checks for each parameter, checks if valid day is given*/
  if (!calendar || !name || day < 1 || day > calendar->days) {
      return FAILURE;
    }

  curr = calendar->events[day - 1];

  while(curr) {
    
    /*Compares current name to parameter name, if they match,
      the out parameter is set to the current event and SUCCESS is returned*/
      if (!strcmp(curr->name, name)) {
	
	/*Only sets event to curr if event!=NULL*/
	if(event){ 
	(*event) = curr;
	}
	  return SUCCESS;
	}

      /* Checks next event */
      curr = curr->next;
    }

  /*If the event was never found, the function will return failure*/
  return FAILURE;
}

/*This function removes a specified event from the calendar, and 
  deallocates the necessary memory to do so, it returns failure
  if parameters are null or the specified even does not exist*/
int remove_event(Calendar *calendar, const char *name) {

  int i;
  Event *curr, *prev = NULL, *find;

  /*Null check parameters*/
  if(!calendar || !name){

    return FAILURE;
  }
  
  /*Returns failure if event does not exist*/
  if(find_event(calendar, name, &find) == FAILURE){
    
    return FAILURE;
  }


  /*Loops through calendar*/
  for(i = 0; i<calendar->days; i++){

    curr = calendar->events[i];
    while(curr){

      /*If event is found*/
      if(!strcmp(curr->name, name)){

	if(!prev){
	  calendar->events[i] = curr->next;
	}
	else{
	  prev->next = curr->next;
	}

	/*If info is not null and free_info_func exists,
	  the function is called*/
	if(curr->info && calendar->free_info_func) {

	  calendar->free_info_func(curr->info);
	}

	/*frees name since it is dynamically allocated*/
	free(curr->name);
	free(curr);
	calendar->total_events--;
	curr = NULL;
      }
      /*Continues loop if event is not found*/
      else{
	prev = curr;
	curr = curr->next;
      }
    }

  }

  return SUCCESS;

}

/*This function returns a pointer to the info parameter of a specified
  event*/
void *get_event_info (Calendar * calendar, const char *name) {
  Event *get;
  
  /*Searches for the event with find_event, and sets that event
    to get if the event is found and returns the info, returns
    null if it is not found*/
  if(find_event (calendar, name, &get) == SUCCESS) {

    return get->info;
  }
  else {

    return NULL;
  }
}


/*This function removes all events for a given day if the calendar 
  is not null and the given day is valid*/
int clear_day(Calendar * calendar, int day) {
  Event *curr, *prev = NULL;

  if(!calendar || day < 1 || day > calendar->days
      || calendar->total_events < 0) {

      return FAILURE;
    }

  curr = calendar->events[day - 1];
  while(curr) {
      prev = curr;
     
      /*If info is not null and free_info_func exists,
	the function is called*/
      if(curr->info && calendar->free_info_func) {

	  calendar->free_info_func(curr->info);
	}

      /*frees name since it is dynamically allocated*/
      free(curr->name);
      calendar->total_events--;
     
      curr = curr->next;
      free(prev);
  }

  calendar->events[day - 1] = NULL;
  return SUCCESS;

}

/*This function removes all event lists in the calendar parameter if
  the given calendar is not null*/
int clear_calendar(Calendar *calendar) {
  int i;
 
  /*Null check calendar parameter*/
  if(!calendar) {

    return FAILURE;
  }
 
  /*Calls clear_day on each day, clearing each event list*/
  for(i = 1; i < calendar->days+1; i++) {

      clear_day(calendar, i);
    }

  calendar->total_events = 0;

  return SUCCESS;
}

/*This function frees all memory associated with the specified calendar,
  it returns FAILURE if the calendar does not exist*/
int destroy_calendar(Calendar *calendar) {

  if(!calendar) {

      return FAILURE;
    }

  /*clear_calendar uses clear_day to free all necessary data in the
    events, so the only dynamically allocated memory left to free is
    name, events, and the calendar itself*/
  clear_calendar(calendar);

  free(calendar->name);
  free(calendar->events);
  free(calendar);

  return SUCCESS;
}
