/*
██████████████████████████████████████████████████████████████████████████████████████████████████████████
██                                                                                                      ██
██  ██████╗  █████╗ ███████╗ █████╗ ███████╗██╗         ██╗    ██╗██████╗  ██████╗ ████████╗ ███████╗   ██
██  ██╔══██╗██╔══██╗██╔════╝██╔══██╗██╔════╝██║         ██║    ██║██╔══██╗██╔═══██╗╚══██╔══╝ ██╔════╝   ██
██  ██████╔╝███████║█████╗  ███████║█████╗  ██║         ██║ █╗ ██║██████╔╝██║   ██║   ██║    █████╗     ██
██  ██╔══██╗██╔══██║██╔══╝  ██╔══██║██╔══╝  ██║         ██║███╗██║██╔══██╗██║   ██║   ██║    ██╔══╝     ██
██  ██║  ██║██║  ██║██║     ██║  ██║███████╗███████╗    ╚███╔███╔╝██║  ██║╚██████╔╝   ██║    ███████╗   ██
██                                                                                                      ██
██                         RAFAEL WROTE THIS FILE                                                       ██
██████████████████████████████████████████████████████████████████████████████████████████████████████████
*/


//VGA -- external functions and variables from labmain, check definition there
extern void draw_point(char color, int x, int y, int thickness);
extern void draw_rect(char color, int x1, int y1, int x2, int y2, int t, int fill);
extern void draw_cursor(int x, int y);
extern int x_mouse;
extern int y_mouse;

//PIO -- external variables from labmain, check definition there
extern volatile int* PIO;
extern char M_SW;
extern char Sidebar;
extern char delete;
extern char Select;
extern char delete_grid;
extern char place;
char select_flag = 0;


//creates Bitmask selector overlay, small rectangular screen where differnt bitmasks can be selected
void Bitmask_Sidebar(unsigned int (*bits)[2]){
    //UI - draws the rect on the left of screen with outline
    draw_rect(0x92, 0, 0, 74, 240, 1, 1);
    draw_rect(0x00, 0, 0, 74, 240, 1, 0);

    //specific color array so that when u hover over a box with the mouse it changes color
    unsigned char overlay_color[8];
    for (int m = 0; m < 8; m++) {
        overlay_color[m] = 0x52; //random color could change
    }

    int selected_box = -1;
    for (int m = 0; m < 8; m++) {
        int y = 8 + m * 29; //specific offset of each bitmask box

        if (x_mouse >= 4 && x_mouse < 68 && y_mouse >= y && y_mouse < y + 20){ //checks if mouse if over a box and if it is sets selected_box to which box its selecting
            selected_box = m;
            overlay_color[m] = 0x29; //changes teh color of hovered bitmaskbox
        }
    }

    // 8 masks, each 64x20 bits.
    static unsigned int masks[8][20][2] = { //_________________________________specific_bitmasks____________________________________
    // 0: Glider (3x3)
    {
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, 
        {0x00000000, 0x80000000},
        {0x00000000, 0x40000000},
        {0x00000001, 0xC0000000},
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}
    },

    // 1: Lightweight Spaceship / LWSS (5x4)
    {
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
        {0x00000002, 0x40000000},
        {0x00000004, 0x00000000},
        {0x00000004, 0x40000000},
        {0x00000007, 0x80000000},
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}
    },

    // 2: Copperhead Spaceship (8x10) - Slower, complex flyer
    {
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
        {0x00000006, 0x60000000},
        {0x00000001, 0x80000000},
        {0x00000001, 0x80000000},
        {0x0000000A, 0x50000000},
        {0x00000008, 0x10000000},
        {0x00000000, 0x00000000},
        {0x00000008, 0x10000000},
        {0x00000006, 0x60000000},
        {0x00000002, 0x40000000},
        {0x00000001, 0x80000000},
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}
    },

    // 3: Acorn Methuselah (7x3)
    {
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
        {0x00000000, 0x40000000},
        {0x00000001, 0x00000000},
        {0x0000000E, 0x60000000},
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}
    },

    // 4: Pulsar (13x13)
    {
        {0,0}, {0,0}, {0,0}, {0,0},
        {0x0000001C, 0x70000000},
        {0x00000000, 0x00000000}, 
        {0x00000042, 0x84000000},
        {0x00000042, 0x84000000},
        {0x00000042, 0x84000000},
        {0x0000001C, 0x70000000},
        {0x00000000, 0x00000000},
        {0x0000001C, 0x70000000},
        {0x00000042, 0x84000000},
        {0x00000042, 0x84000000},
        {0x00000042, 0x84000000},
        {0x00000000, 0x00000000},
        {0x0000001C, 0x70000000},
        {0,0}, {0,0}, {0,0}
    },

    // 5: Heavyweight Spaceship / HWSS (7x5)
    {
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
        {0x00000003, 0x00000000},
        {0x00000010, 0x40000000},
        {0x00000000, 0x20000000},
        {0x00000008, 0x20000000},
        {0x00000007, 0xE0000000},
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}
    },

    // 6: Gosper Glider Gun (36x9)
    {
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
        {0x00000000, 0x02000000},
        {0x00000000, 0x0A000000},
        {0x00000030, 0x3000C000},
        {0x00000044, 0x3000C000},
        {0x00030082, 0x30000000},
        {0x0003008B, 0x0A000000},
        {0x00000082, 0x02000000},
        {0x00000044, 0x00000000},
        {0x00000030, 0x00000000},
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}
    },

    // 7: Diehard (8x3)
    {
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
        {0x00000000, 0x20000000},
        {0x0000000C, 0x00000000},
        {0x00000004, 0x70000000},
        {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}
    }
    }; //______________________________________________________________________________


    //DRAW the boxes and the bitmasks
    for (int i = 0; i < 8; i++) {
        int y = 8 + i * 29;
        for (int h = 0; h < 20; h++) {
            for (int x = 0; x < 2; x++) {
                for (int b = 0; b < 32; b++) {
                    if (masks[i][h][x] & (0x1u << (31 - b)))
                        draw_point(0xFF, x*32+b+4, h+y, 1);
                    else
                        draw_point(overlay_color[i], x*32+b+4, h+y, 1);
                }
            }
        }

        // Outline around the full 64x20 mask
        draw_rect(0x00, 3, y - 1, 68, y + 20, 1, 0);
        //draw_rect(0x52, 6, y + 2, 68, y + 22, 1, 1);
    }

    if(select_flag && (selected_box != -1)){
        //if select_flag is true which means the button is pressed and we are currently hovering over a box, it writes to labmain's bitmask the selected bitmask
        for (int h = 0; h < 20; h++) {
            bits[h][0] = masks[selected_box][h][0];
            bits[h][1] = masks[selected_box][h][1];
        }
        Sidebar = 0; //closes sidebar
        Select = 1; //Select flag turns on meaning the bitmask overlay appears on screen
    }
    else
        select_flag = 0; //turns off button flag since its not hovering over anything.
}



void buttonfunc(void){ //This is basically just simple funcictionality for button
    //Built in button
    static int pressed = 0; //pressed variable checks if the button has alreayd been pressed so its functionality only repeates every separate button press
    if(0x1 & *((volatile int*)0x040000d0)){ //check the buttons current value in the memory
        if(!pressed){ //button funct
            place = (place+1)%2;
            delete = 0;
        }
        pressed = 1;
    }
    else{ //reset pressed to 0 if it button isnt pressed anymore.
        pressed = 0;
    }
    
    //joystick button
    static int jt_pressed = 0;
    if(!(0x100 & PIO[0])){
        if(!jt_pressed){
            M_SW = (M_SW+1)%2;
        }
        jt_pressed = 1;
    }
    else{
        jt_pressed = 0;
    }

    //external button 1
    static int pressed1 = 0;
    if(!(0x200 & PIO[0])){
        if(!pressed1){
            Sidebar = (Sidebar+1)%2;
            M_SW = 1;
            Select = 0;
        }
        pressed1 = 1;
    }
    else{
        pressed1 = 0;
    }

    //external button 2
    static int pressed2 = 0;
    if(!(0x400 & PIO[0])){
        if(!pressed2){
            select_flag = 1;
        }
        pressed2 = 1;
    }
    else{
        pressed2 = 0;
    }

    //external button 3
    static int pressed3 = 0;
    if(!(0x1000 & PIO[0])){
        if(!pressed3){
            delete = (delete+1)%2;
            place = 0;
        }
        pressed3 = 1;
    }
    else{
        pressed3 = 0;
    }

    //external button 4
    static int pressed4 = 0;
    if(!(0x800 & PIO[0])){
        if(!pressed4){
            delete_grid = 1;
        }
        pressed4 = 1;
    }
    else{
        pressed4 = 0;
    }
}