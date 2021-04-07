#include "music.h"

Music::Music()
{

}

void Music::Play_Alarm_Music()
{
    system("aplay /home/Wolf/Alarm.wav");
}
