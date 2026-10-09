/*
███╗   ███╗██╗██╗  ██╗███████╗██████╗
████╗ ████║██║╚██╗██╔╝██╔════╝██╔══██╗
██╔████╔██║██║ ╚███╔╝ █████╗  ██║  ██║
██║╚██╔╝██║██║ ██╔██╗ ██╔══╝  ██║  ██║
██║ ╚═╝ ██║██║██╔╝ ██╗███████╗██████╔╝
╚═╝     ╚═╝╚═╝╚═╝  ╚═╝╚══════╝╚═════╝

████████████████████████████████████████████████████████████████
██                                                            ██
██            ARNAV AND RAFAEL WROTE THIS FILE                ██
██                                                            ██
████████████████████████████████████████████████████████████████
*/



extern void print(const char*);
extern void print_dec(unsigned int);
extern void display_string(char*);
extern void print_hex32(unsigned int x);

//counter functions from Counters.S
typedef unsigned long long int uint64_t;
extern uint64_t mcycle(void);
extern uint64_t minstret(void);
extern uint64_t mhpmcounter3(void);
extern uint64_t mhpmcounter4(void);
extern uint64_t mhpmcounter5(void);
extern uint64_t mhpmcounter6(void);
extern uint64_t mhpmcounter7(void);
extern uint64_t mhpmcounter8(void);
extern uint64_t mhpmcounter9(void);
extern void clear_counters(void);

//external IO functs
extern void buttonfunc(void); //ALL buttons and button flags are handled by this function
extern void Bitmask_Sidebar(unsigned int (*bits)[2]); //Bitmask side Bar for bitmask selection

//VGA -- functions to draw on the VGA
extern void draw_point(char color, int x, int y, int thickness);
extern void draw_rect(char color, int x1, int y1, int x2, int y2, int t, int fill);
extern void draw_cursor(int x, int y);



//IO
//0x04000020–0x0400003F
extern void enable_interrupt(void); //assembly function that enables timer interupts
volatile int* tp = (volatile int*)0x04000020; //timer IO adress
volatile int* PIO = (volatile int*)0x040000e0; //GPIO pins adress
volatile char *VGA = (volatile char*) 0x08000000; //VGA IO Adresses
volatile int *VGA_CTRL = (volatile int*) 0x04000100;//VGA control registers 0 for pointer to frame buffer, 1 for control register


//colors
#define Bd_C 0x49 //background color

//IO devices variables
volatile unsigned Vrx; //Joystick X axis value
volatile unsigned Vry; //Joystick Y axis value
volatile unsigned P1; //Potentiomere 1 value
volatile unsigned P2; //Potentiometer 2 value

//game variables
#define MaxSpeed 200 //how long until the next generation occurs
#define GRID_W 256 //Width of the cell grid
#define GRID_H 180 //Height of the cell grid
#define grid_width_words ((GRID_W + 31) / 32) // Automatically calculate how many 32-bit words are needed per row
#define CURSOR_SPEED 3  //how fast the joystick moves the cursor and the grid

int x_shift = 0; //cell grid horizontal shift
int y_shift = 0; //cell grid vertical shift
int x_mouse = 180; //mouse x position
int y_mouse = 120; //mouse y position
char paused = 1; //Flag to enable next cell generation calculations
int scale = 1; //zoom in size blocks

//Button Functions
char M_SW = 0; //enable and disable cursor
char Sidebar = 0; //enable and disable Sidebar
char Select = 0; //select a bitmask of cells to place/cell overlay
char delete_grid = 0; //clear cellular grid
char place = 0; //place block/selected bitmask
char delete = 0; //delete cell

//game funct (Updated for 32-bit engine)
extern void populateGrid(int grid_w, int grid_h, int grid_width_wordss, unsigned int grid[grid_h][grid_width_wordss], char fill);
extern void step_simulation_32(int grid_h, int grid_width_wordss, unsigned int src[grid_h][grid_width_wordss], unsigned int dst[grid_h][grid_width_wordss]);

//IO devices update
void TimerInterupt(){ //Rafael WROTE THIS
    static unsigned addr = 0; //static variables used for analog data collection from arduino
    static unsigned raw[6]; //storage for this data
    static int timeoutcount = 0; //counter for cell generation
    static int speedcount = MaxSpeed; //the thing the above variable counts to -- how fast cell generaiton occurs

    raw[addr] = PIO[0] & 0x1F; //read first 5 PIO pins and set the current adress of raw equal to it.
    if (addr >= 5) { //this upates the values of Vrx, Vry, P1 and P2 with all the analog values we saved in the function above
        Vrx = raw[0] << 5;
        Vry = raw[1] << 5;
        P1  = raw[2] | (raw[3] << 5);
        P2  = raw[4] | (raw[5] << 5);
        //update game variables
        if (P1 < 0) P1 = 0;
        if (P1 > 600) P1 = 600;
        speedcount = MaxSpeed - (P1 * (MaxSpeed - 1)) / 600;
    }
    addr = (addr + 1)%6; //increase adress by 1 so that next analog variable is checked
    PIO[0] = (PIO[0] & ~0xE0) | (addr << 5); //Change the output pins 6-8 to the current adress value
    if(++timeoutcount >= speedcount){ //______check that allows for variable speed change in cell generation
        timeoutcount = 0;
        if(speedcount < MaxSpeed-10){ //check if u want to completely pause cell generation
            paused = 0; //enable flag for next cell generation -- i.e. unpause the grid update.
        }
    }
}

//initializing IO
void timer_init(void){ //Rafael WROTE THIS
    tp[2] = 0x93E0; // these two lines enable a 100ms clock cycle //Lower half of the 32 bit timer register
    tp[3] = 0x4; //upper half of the 32 bit timer register
    tp[4] = tp[5] = 0; //clear current timer count
    tp[0] = 0x0; //clear interupt flag
    tp[1] = 0x7; //enable interupt, "timer-continue" and turns timer on-
}

void GPIO_init(void){ //Rafael WROTE THIS
    PIO[1] = 0xE0; //pins 6-8 output, the rest input
    PIO[0] = 0; //clear current values
}

void labinit(void) //initializes interups and call timer/PIO functs //Rafael WROTE THIS
{
    timer_init();
    GPIO_init();
    enable_interrupt();
}

void handle_interrupt(unsigned cause) { //Rafael WROTE THIS
    if(cause == 16){ //if interupt is due to timer cause = 16.
        tp[0] &= ~0x1; //clear timer interupt flag
        TimerInterupt(); //calls TimerInterupt which updates analog values(joystick & potentiomers) one by one.
    }
}

//VGA funcs
void clear_screen(){ //set all pixels on teh current frame (320 by 240) to the background color //Rafael WROTE THIS
    for (int i = 0; i < 320*240; i++){
        VGA[i] = Bd_C;
    }
}

static inline int floor_i(float v) { //important function floor used to center the grid without cells being shifted //Arnav WROTE THIS
    int i = (int)v;
    return (v < (float)i) ? i - 1 : i;
}


void draw_grid(unsigned int (*current_grid)[grid_width_words]) { //Arnav and Rafael WROTE THIS
    //integer zoom
    int target = 1 + (P2 >> 5); 
    int scale_lower_bound = (scale - 1) << 5;
    if (P2 >= scale_lower_bound + 32 + 4 || P2 < scale_lower_bound - 4) scale = target; //to not change scale too often, only change it if the potentiometer is far enough away from the current scale value

    //view center snapped to a whole grid cell
    int center_x = floor_i(GRID_W/2.0f + x_shift);
    int center_y = floor_i(GRID_H/2.0f + y_shift);

    //Here the variables are created for the boudaries of the drawing grid
    int gx0 = center_x - 160/scale - 1; if (gx0 < 0) gx0 = 0;
    int gx1 = center_x + 160/scale + 1; if (gx1 > GRID_W) gx1 = GRID_W;
    int gy0 = center_y - 120/scale - 1; if (gy0 < 0) gy0 = 0;
    int gy1 = center_y + 120/scale + 1; if (gy1 > GRID_H) gy1 = GRID_H;

    // Loop Y outer, Word inner, Bit innermost
    for (int y = gy0; y < gy1; y++) { //loop from gy0 to gy1
        for (int x = gx0/32; x <= gx1/32; x++) { //loop from gx0 to gx1 but divide by 32 cuz each unsigned integer in the grid holds 32 cells
            
            unsigned int chunk = current_grid[y][x]; //take the 32 cell of the row we are going to print
            //loop through that row and draws to the screen
            for (int bit = 0; bit < 32; bit++) {
                if (chunk & 1) { // Check if the lowest bit is a 1
                    draw_point(0xFF, (x * 32 + bit - center_x) * scale + 160, (y - center_y) * scale + 120, scale); // 0xFF = white
                }
                chunk >>= 1; // Shift right to print the next cell on that word
            }
        }
    }
}


/* MAIN */
int main ( void ) { //Arnav and Rafael WROTE THIS
    //Inbuilt IO setup
    labinit();

    //screen setup
    clear_screen(VGA);
    VGA_CTRL[1] = (unsigned int) (VGA);
    VGA_CTRL[0] = 0;

    //Initialize bitmask to all 0s
    unsigned int bitmask[20][2];
    for (int i = 0; i < 20; i++){
        bitmask[i][0] = 0;
        bitmask[i][1] = 0;
    }

    // Initialize the 32-bit arrays at and offset cuz we were running into issues where both grids occupied the same D-stack indexes
    static unsigned int mem[2*GRID_H*grid_width_words + 256];
    unsigned int (*current_grid)[grid_width_words] = (void*)mem;
    unsigned int (*next_grid)[grid_width_words] = (void*)(mem + GRID_H*grid_width_words + 256);

    populateGrid(GRID_W, GRID_H, grid_width_words, current_grid, 1); //Populate the initial grid with a pattern using a seed value. Grid W is the width of the grid, Grid H is the height of the grid, and Words X is the number of 32-bit words per row.

    while (1)
    {   
        //update VGA pointer to next frame
        VGA = (volatile char*)(((int)VGA & ~0x3FFFF) | ((((int)VGA & 0x3FFFF)+320*240)%(320*480)));
        clear_screen();
        
        if (!paused) { //generates next grid only if paused flag is off
            paused = 1;
            // Generate next frame
            step_simulation_32(GRID_H, grid_width_words, current_grid, next_grid); // Compute the next state of the grid using the current state and store it in the next grid. GRID_H is the height of the grid, grid_width_words is the number of 32-bit words per row, current_grid is the current state of the grid, and next_grid is where the next state will be stored.
            // Swap the pointers so next_grid becomes the source for the next frame
            unsigned int (*temp)[grid_width_words] = current_grid;
            current_grid = next_grid;
            next_grid = temp;
        }
        

        if(delete_grid){ //Clear grid button functionality
            populateGrid(GRID_W, GRID_H, grid_width_words, current_grid, 0);
            delete_grid = 0;
        }

        draw_grid(current_grid); //draw grid function


        buttonfunc(); //Button functionality, switch between cursor or screen shift and other things
        if(M_SW){ //M_SW swtiches between the cursor and moving the screen. 
            x_mouse = (x_mouse + ((int)Vrx - 480) / 400 * CURSOR_SPEED + 320) % 320;
            y_mouse = (y_mouse - ((int)Vry - 480) / 400 * CURSOR_SPEED + 240) % 240;
            draw_point(0x80, floor_i((x_mouse - 160) / (float)scale) * scale + 160, floor_i((y_mouse - 120) / (float)scale) * scale + 120, scale); //highlights the cell the mouse is currently poiting to
            if(Sidebar){ //if Sidebar flag is enabled(controlled by button) the sidebar appears on screen
                Bitmask_Sidebar(bitmask);
            }
            if(Select){ //if Select flag is enabled(if u select a bitmask on the sidebar) the overlay for that bitmask appears on screen. Note that the array bitmask is changed inside Bitmask_Sidebar
                for (int h = 0; h < 20; h++) {
                    for (int x = 0; x < 2; x++) {
                        for (int b = 0; b < 32; b++) {
                            if (bitmask[h][x] & (0x1 << (31 - b)))
                                draw_point(0xB6, floor_i((x_mouse - 160) / (float)scale) * scale + 160 + (x*32 + b - 32) * scale, floor_i((y_mouse - 120) / (float)scale) * scale + 120 + (h - 10) * scale, scale);
                        }
                    }
                }
            }
            if(place){ //if place flag is enabled(if u select a bitmask on the sidebar) the overlay for that bitmask appears on screen.
                int mx = floor_i(GRID_W/2.0f + x_shift) + floor_i((x_mouse - 160) / (float)scale); //xpos in the zoomed in graph
                int my = floor_i(GRID_H/2.0f + y_shift) + floor_i((y_mouse - 120) / (float)scale); //ypos in the zoomed in graph
                if(Select){ //if there is a current overlay selected, then we place that
                    for (int h = 0; h < 20; h++) for (int x = 0; x < 2; x++) for (int b = 0; b < 32; b++) if (bitmask[h][x] & (0x1 << (31 - b))) {
                        int gx = mx + (x*32 + b - 32);   // same offsets as the overlay
                        int gy = my + (h - 10);
                        if (gx >= 0 && gx < GRID_W && gy >= 0 && gy < GRID_H)
                            current_grid[gy][gx >> 5] |= 1 << (gx & 31);
                    }
                    place = 0;
                }
                else{ //otherwise, we just place a pixel in the current mouse position, notice that we dont clear the paused flag here so that u can keep drawing each pixel.
                    if (mx >= 0 && mx < GRID_W && my >= 0 && my < GRID_H)
                        current_grid[my][mx >> 5] |= 1 << (mx & 31);
                }
            }
            if(delete){ //same thign as the place pixel but it deletes a pixel
                int mx = floor_i(GRID_W/2.0f + x_shift) + floor_i((x_mouse - 160) / (float)scale); //xpos in teh zoomed in graph
                int my = floor_i(GRID_H/2.0f + y_shift) + floor_i((y_mouse - 120) / (float)scale); //ypos in teh zoomed in graph
                if (mx >= 0 && mx < GRID_W && my >= 0 && my < GRID_H)
                    current_grid[my][mx >> 5] &= ~(1 << (mx & 31));
            } 
            draw_cursor(x_mouse,y_mouse); //draws cursor --- Check VGA_Functions
        }
        else{ //if M_SW is not on, we just change x_shift and y_shift so the whole grid moves
            Select = 0; 
            place = 0;
            delete = 0;
            x_shift += (((int)Vrx-480)/400)*CURSOR_SPEED;
            y_shift -= (((int)Vry-480)/400)*CURSOR_SPEED;
        }
        
        //update VGA pointer to our current VGA pointer and update the screen
        VGA_CTRL[1] = (unsigned int) (VGA);
        VGA_CTRL[0] = 0;

        for (int i = 0; i < 10000; i++) //small delay -- lecture slide's code
            asm volatile ("nop");
    }
}