#include <iostream>
#include <cstdlib>
#include <process.h>
#include <string>
#include <vector>
#include <dos.h>
#include <i86.h>
#include <conio.h>

std::vector<bool> isprogram;
std::vector<std::string> execs;
std::vector<std::string> itemNames;
volatile unsigned char *video = (volatile unsigned char *)0xB8000;

void drawInterface() {
    for (int i = 0; i < 80 * 25 * 2; i += 2) { // CLEANS THE SCREEN WITH BLUE
        video[i] = ' ';
        video[i + 1] = 0x11;
    }

    for(int i = 0; i < 160; i += 2) {    // PAINTS THE HEADER
        video[i] = ' ';
        video[i + 1] = 0xEE;
    }

    std::string title = "LIST";
    for(size_t i = 0; i < title.length(); i++) { // DRAWS THE TEXT
        video[36 + (i * 2)] = title[i];
        video[37 + (i * 2)] = 0xE1;
    }

    int linecounter = 3;
    for(size_t i = 0; i < itemNames.size(); i++) { // SELECTED EFFECT
        int actualchar = linecounter * 160;
        for(size_t j = 0; j < itemNames[i].length(); j++) {
            video[actualchar + (j * 2)] = itemNames[i][j];
            video[actualchar + (j * 2) + 1] = 0x1E;
        }
        linecounter++;
    }
}

void addItemOnce(std::string name, std::string POC, std::string exec) {
    execs.push_back(exec);
    itemNames.push_back(name);
    isprogram.push_back(POC != "command");
}

void redrawList(int selecteditem) {
    for(size_t i = 0; i < itemNames.size(); i++) {
        int linePos = (3 + (int)i) * 160;
        unsigned char color = (i == (size_t)selecteditem) ? 0xE1 : 0x1E;

        for(size_t j = 0; j < itemNames[i].length(); j++) {
            video[linePos + (j * 2) + 1] = color;
        }
    }
}


int main() {
    int selecteditem = 0;

    // ACTIVATES BRIGHT VGA BACKGROUNDS
    inp(0x3DA);
    outp(0x3C0, 0x10);
    unsigned char registerer = inp(0x3C1);
    inp(0x3DA);
    outp(0x3C0, 0x10);
    outp(0x3C0, registerer & ~0x08);
    inp(0x3DA);
    outp(0x3C0, 0x20);
    // ADD YOUR ITEM HERE WITH |NAME|PROGRAM/COMMAND|COMMAND
    
    addItemOnce("compile list", "command", "YOUR COMPILE COMMAND HERE");
   addItemOnce("edit", "program", "edit");
    addItemOnce("Return to DOS", "command", "r0");
    //==================================================
    drawInterface();
    redrawList(selecteditem);

    while(true) {
        if(kbhit()) {
            int key = getch(); // GET ACTUAL PRESSES KEYBOARD

            if(key == 'w' || key == 'W' || key == 0x91) {
                if(selecteditem > 0) {
                    selecteditem--; // ITEM UP
                }
            }
            else if(key == 's' || key == 'S' || key == 0x1F) {
                if(selecteditem < (int)execs.size() - 1) {
                    selecteditem++; // ITEM DOWN
                }
            }
            else if(key == 0x0D || key == 0x9C) { // ENTER
                if(execs[selecteditem] == "r0") {
                    break; // RETURN TO DOS
                }

             
                std::system("cls");

                if(isprogram[selecteditem]) {
                    std::system(execs[selecteditem].c_str());
                } else {
                    std::string fullCmd = execs[selecteditem];
                    std::system(fullCmd.c_str());
                }

                std::cout << "\nPress any key to return to List...";
                getch();

                drawInterface();
            }

            redrawList(selecteditem);
        }
    }

    //  CLEANS THE SCREEN FOR DOS
    std::system("cls");

    return 0;
}
