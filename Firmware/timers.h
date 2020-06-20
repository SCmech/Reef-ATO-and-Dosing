
struct event
 {
  const byte pump; //3 is calcium 4 is magnesium
  int duration; //duration of the event in millis
  int hour; //
  int minute; //time event occurs
 };

event allEvents[12] = {
{3, 20000, 16, 43}, //C0
{3, 20000, 16, 43}, //C1
{3, 20000, 16, 43}, //C0
{4, 20000, 16, 43}, //C1
{4, 20000, 16, 43}, //C0
{4, 20000, 16, 43}, //C1
};

//not implemented yet
typedef struct Menu
{
  const byte minLine; //starting line of the 
  const byte maxLine;
  const byte startLine; //where the cursor starts?
  boolean hasCursor; //1 is yes, 0 is no
};

//Menu menu0 = {1, 3, 0};
//Menu menu1 = {0, 6, 1};
