#include "stories.h"
//#define LAST_STORY 1
//int stories_popularity[LAST_STORY];
int story=-1;//Set him to read
int story_pause[NO_STORIES]={7*1000, 4*1000, 7*1000};//Pause for 7 seconds
int story_passages[NO_STORIES]={15,22,12};
int story_time;
float story_length=0;
int which_story=-1;//No story
//Introduction
char *introduction1="I would recommend remaining silent while I tell the story.";
char *introduction2="Come,gather round. The story I am about to tell you is \"%s\". It is %f minuutes long.";
//Story Names
char * story_name [NO_STORIES]={"A Tale in the Desert", "The Battle at the Well"};
//char story_passage [number of stories][number of passages][number of charcaters];//Max 10 minutes long.
char * story_passage [NO_STORIES][70]={{"The story of the legendary duo of Harvy and Grim is a strange one. They were both born on the same night, just as a meteor was seen in the sky.",
                                      "Normally, such an sign would be taken as a lucky omen, but in their childhoods, both seemed to be cursed with a streak of misfortune.",
                                      "Grim, child of a dwarven mining family in Mynadar, was expected to join the family trade. However, he had a problem - extreme claustrophobia.",
                                      "He was unable to enter ANY enclosed space at all - not even his own house, once he had grown big enough to feel cramped in it.",
                                      "So, with the loving but bewildered support of his family, Grim began to be a vagabond and a wanderer in his youth, always living out in the open.",
                                      "Harvy, on the other hand, was the only child of an extremely wealthy and influential family in Corren Town, and was raised in a palatial home.",
                                      "However, his parents died at an early age, leaving him orphaned. Harvy was forced to bear the burden of managing his father's business.",
                                      "He soon suffered a nervous breakdown and was sent to Isla Prima to recover. When he got back, he had gotten a taste for the wilderness.",
                                      "He sold off his father's vast business enterprise, and invested all the money into exploring the wilderness, particularly Kamara Desert.",
                                      "Harvy began to be interested in mineralogy, and squandered most of his family's fortune in his studies. He set up permanent shop in Kamara Desert.",
                                      "Grim, on the other hand, had developed a network of extensive contacts in his wanderings. One day, he decided to explore the Kamara Desert.",
                                      "A few days later, he had collapsed from exhaustion in the swirling sands. Harvy found him, and dragged him back to the nearest oasis.",
                                      "During Grim's recovery, they became fast friends. Harvy showed Grim some of his finds - an as-yet-unknown element he called 'mercury',",
                                      "and a glass-making process he had developed using melted quartz. Grim, realising the potential of these discoveries, talked Harvy into a partnership.",
                                      "From then on, the duo wandered the desert, with Harvy continuing his studies into minerals, and Grim using his contacts to sell Harvy's discoveries.",
                                      "And the rest, as they say, is history...",
                                     NULL},
                                     {":Clears his throat",
                                      "All know the tale of brave Trik and how he won his name,",
                                      "But few are left who know the truth, of the battle that brought him fame.",
                                      "'Twards the bustling city of Portland an Orcish horde did march,",
                                      "Yet few tale-tellers would return, to rue the day they left 'neath their cavern's arch.",
                                      "An ambush set 'twixt Desert and Town, on dusty trails thought their own,",
                                      "Trik and his brave band rained death down, and so won renown.",
                                      "The tribesmen fled into the hills, the Orcs in hot pursuit",
                                      "(unmindful of their dying comrades, or their own fate to shortly follow suit.)",
                                      "For three more days they fought on the dusty canyon trails, a deadly game of cat versus mouse,",
                                      "Until at last, tired and sore, Trik's brave men fell back towards home, and fortified Trik's own house.",
                                      "One hundred men remained, to face over twice that the Orcish number,",
                                      "And as guttural snarls echoed cross the village square, in the distance, men heard thunder.",
                                      "Whilst lightning flashed and rain poured down, Trik's friends battled their foe.",
                                      "They fought valiantly, that cannot be denied, yet the orcish number ground them down, even so...",
                                      "Until they heard their loved ones' screams as the orcs found where they hid...",
                                      "\"Once more, ATTACK!\" urged Trik, and so his stalwarts charged as he did.",
                                      "Their blades were slick with Orcish blood, but still their numbers fell,",
                                      "A scant score rallied to Trik where he battled near the village well.",
                                      "His wild eyes found the Orc chieftain and Trik surged yet again into the fray,",
                                      "Certain if he killed the Orc's driving force, his battered men could still carry the day.",
                                      "As Trik held the chieftain's severed head, the last orcs fled with thier tails 'tween their legs...",
                                      "But though they drank from the cup of Victory, Trik, and his remaining ten, tasted but the bitterest of dregs.",
                                     NULL},
                                     {"With the dawn, Thurumbar came awake. Today was here! Praise Aluwen!",
                                      "They, the elves of Trinwood, were leaving - departing for the ancient home, Forest of the Fall.",
                                      "Never more would they live in fear of the monstrosities or Vermor, nevermore would fear prowl at their doors!",
                                      "Long abandoned had been the Ancient Forest, though to no purpose. New purpose would it serve them, now.",
                                      "He made his morning devotion to Aluwen, unhurriedly, then strode forth to lead his followers in prayer.",
                                      "As he led the prayer, he gazed upon his army, small and resolute. He would not, could not fail them.",
                                      "Then, as the first solar rays touched the trees, he gazed at the Fair Vale, as with new eyes. Would they ever return?",
                                      "They gathered their supplies, seven score of the hope of a race which time had left behind.",
                                      "Almost as one, they set out from their Vale, their home. Though the supplies weighed their bodies down, they seemed weightless. They were free!",
                                      "Those of Mortos seed and his servants would be left behind. The Old Forest, the shield of Aluwen would protect them.",
                                      "To the north they marched, with purpose, but also with doubts. Would? Could? What if?",
                                      "But as they walked, all of their fears evaporated, and the passed beyond the castle, to the mountains, marching off into the dawn.",
                                     NULL}};
                                     
char * conclusion="I hope you enjoyed the story. Please come again sometime.";

void init_story(const char *data)
{
     //Randomization didn't seem to work :/ so we use user-defined

     if(!strncasecmp(data, "Harvy&Grim",10))which_story=0;
     else if(!strncasecmp(data, "Trik",4))which_story=1;
     else if(!strncasecmp(data, "FOTF",4))which_story=2;
     
     else return;       
       
     story=0;//Set him to read the story
     read_intro();  
     return;
}    

/*void init_my_story(char * story_name)
{
    sprintf("%s.dat", story_name);
    //Load story --How can we load this phrase by phrase?
    //For now, we'll specify the number of phrases+the length of each phrase
    //In the header, then before each phrase
 
    
   
}*/    

void read_intro()
{
    //calc_story_time(which_story);
    send_raw_text("@%s", introduction1);
    //send_raw_text(introduction2, story_name[which_story], ((story_passage[which_story]) * (story_pause[which_story]/1000)/*/60*/));
    story_time=(SDL_GetTicks())+((story_pause[which_story])*2);
    return;
}

void read_conclusion()
{
    send_raw_text("@%s", conclusion);
    return;
}        

void read_story()
{
    log_info ("Reading passage %d of story #%d\n", story, which_story);
    send_raw_text("@%s", story_passage[which_story][story++]);
    if (!story_passage[which_story][story]) {
        story=-1;//Don't read
        story_time=0;//Just in case...
        which_story=-1;//Just in case
        read_conclusion();
        return;
    }    
    story_time=(SDL_GetTicks())+story_pause[which_story];
    return;
}

/* BROKEN */
void calc_story_time(int STORY) //Need more stories first...
{
    story_length=(*(float *)((story_passages[STORY]) * (story_pause[STORY]/1000)/60));
}
