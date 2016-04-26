#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <cstdlib>

using namespace std;


void showADC(int ADC_fired_amount, fstream &readev2);

void NOTshowADC(int ADC_fired_amount, fstream &readev2);

bool isValidFilename(char* filename);

void  skipUnwant(int event_start, fstream &readev2);

void  showingWanted(int event_start, int N_Event_Set, fstream &readev2);

void  __showVersion();

int Display_total_set(fstream & readev2);




int main( int argc, char *argv[] )  // One can use linux command line to input the filename
{

   __showVersion();  // showing version and update time.



   // Logic switch for different argc
   //------------------------------------------------------------|

    bool set_default;

    bool set_range;

    bool set_test = false; // for testing mode, just one argument
    //-----------------------------------------------------------|



    //  set up the mode, according to argument number.
    //-------------------------------------------------------------------------------------------------------------|
    if ( argc < 2)     { set_test = true; cout << "enter testing mode. "<<endl;}  // ex. key in ./read_ev2_tai_2

    if ( argc == 2 )   { set_default = true;  set_range = false; } // key in ./read_ev2_tai_2 filename

    if ( argc == 3 )   { cout <<"Error: missing the \"stating point\" or \"amount of events\"\n"; }

    if ( argc == 4 )   { set_default = false; set_range = true;}   // key in ./read_ev2_tai_2 filename 1 20

    if ( argc > 4 )    { cout <<"Error: taking too many arguments! \n"; }
    //-------------------------------------------------------------------------------------------------------------|




    // Dealing the input filename
    //--------------------------------------|

    char* filename = new char [30];

    if ( set_test ) { strcpy(filename,"run142.ev2") ; } // in testing mode, it will read run142.ev2

    else { strcpy(filename,argv[1]) ; } // in other mode, it will read the argv[1] string.

    //--------------------------------------|




    int event_start;   // for which No.event set to start.

    int N_Event_Set;   // for the number of set wanted to read.


    fstream readev2;

    readev2.open(filename, ios::in|ios::binary);

    //------------------------------------------------------------------------|check opening the file .
    if(readev2.fail()) { cout << "Error: having problem to open "<<filename<<". "<<endl; return 0; }




    if ( set_test ) { event_start = 0;  N_Event_Set = 10;} //argc == 1 // start from human 1, computer 0

    if ( set_range ) // argc = 4
    {

        if(atoi( argv[2]) == 0 ) { cout << " It should start from 1....Aborted \n\n ";  return 0;}
        if(atoi( argv[2] ) > 0 )
           { event_start = atoi( argv[2] ) - 1; } // human starts from 1, but computer starts from 0

        N_Event_Set = atoi( argv[3] );
    }

    if ( set_default ) // argc = 2
    {

        cout << "\n " <<filename<<" has \""<<Display_total_set(readev2) <<"\""<<" event sets\n\n";
    }






    if( set_range) {  skipUnwant(event_start,readev2); }

    if( set_range) { showingWanted(event_start, N_Event_Set, readev2); }


    readev2.close();
    delete [] filename;


    return 0;
}



//---------------------------------------------------------------------------------------Function
//
//  Calculating total event sets in .ev2 file (for argc = 2 )
//
int Display_total_set(fstream & readev2)
{
    // DATE:: Sep. 4 2012, I add this new function.

    int x1;    // serve as the first ch,
                // it works as ADC_fired_amount variable
                // as read from .ev2 data....for conversion date type.
    char x1_temp;
    unsigned y1_temp;

    char*   x2; // for ADC_Num
    short*  x3; // for ADC_Channel

    int total_counting = -1; // initialization here.

    while ( !readev2.eof() )
    {
        readev2.read( &x1_temp, sizeof(x1_temp));

        if(x1_temp <0) { x1_temp +=256;}

        x1 = static_cast<int>(x1_temp);
        x2 = new char [x1];
        x3 = new short [x1];

        if( x1 != 0 )
        {
            for(int i = 0; i < x1; i++)
            {
                readev2.read( &x2[i], sizeof(x2[i]));
                readev2.read( reinterpret_cast<char*>(&x3[i]), sizeof(x3[i]) );
            }

            delete [] x2; delete [] x3;

            readev2.read( &x1_temp, sizeof(x1_temp));
        }




        total_counting++;

    }

    return total_counting;
}

//---------------------------------------------------------------------------------------Function
//  (Not in use)
//  Just to get data, use cout to show ADC # and channel #
//
void showADC(int ADC_fired_amount, fstream &readev2)
{

        bool bad_event = false;

        char* ADC_Num = new char [ADC_fired_amount];
        //----------------------------------------------------ps. create a dynamic array for storage 1 byte date. (256)

        short* ADC_Channel = new short [ADC_fired_amount];
        //----------------------------------------------------ps. create a dynmic array for storage 2 byte date.


        char temp;

        int dummy_temp;
        //----------------------------------------------------ps. these two variable just for temp use.


        // first for loop is for check bad event only..
        for(int i = 0; i < ADC_fired_amount; i++)
        {
            readev2.read( &ADC_Num[i], sizeof(ADC_Num[i]) );

            dummy_temp = static_cast<int>(ADC_Num[i]);

            if (dummy_temp == 0)
            {
                bad_event = true; // if we have ADC num = 0, it is a bad event.
            }

            //------------------------------------------------------------------------------
            //   to read the channel, need two bytes.
            //------------------------------------------------------------------------------
            readev2.read(reinterpret_cast<char*>(&ADC_Channel[i]),sizeof(ADC_Channel[i]));

            if (ADC_Channel[i] <= 0 /*|| ADC_Channel[i] > 8192*/ ) { bad_event = true; }

            // if channel is negative, then it is a bad event.


        } // end of first for loop.




        if ( !bad_event ) // if without bad event.
        {
            // the second for loop is for displaying.
            for(int i = 0; i < ADC_fired_amount; i++)
            {
                //--------------------------------------------------------------
                //  the following codes is from proventing have negative ADC num
                //--------------------------------------------------------------

                if( static_cast<int>(ADC_Num[i]) > 0 )
                {
                    cout <<"(#:" <<static_cast<int>(ADC_Num[i]);
                }
                if ( static_cast<int>(ADC_Num[i]) < 0)
                {
                    cout <<"(#:" <<static_cast<int>(ADC_Num[i]) + 256;
                }

                cout<<"  Ch:"<< ADC_Channel[i]<<")  ";
            }
        }
        else if( bad_event)
        {
            cout << "bad events....ADC = 0 or Channel <=0";
        }

        cout <<"\n\n";


        readev2.read( &temp, sizeof(temp));

        delete [] ADC_Num;
        delete [] ADC_Channel;
    }


//---------------------------------------------------------------------------------------Function
//  (Not in use)
//  Just to get data, but not using cout to show result
//
void NOTshowADC(int ADC_fired_amount, fstream &readev2)
{

        char* ADC_Num = new char [ADC_fired_amount];
        //----------------------------------------------------ps. create a dynamic array for storage 1 byte date. (256)

        short* ADC_Channel = new short [ADC_fired_amount];
        //----------------------------------------------------ps. create a dynmic array for storage 2 byte date.


        char temp;

        int dummy_temp;
        //----------------------------------------------------ps. these two variable just for temp use.


        for(int i = 0; i < ADC_fired_amount; i++)
        {

            readev2.read( &ADC_Num[i], sizeof(ADC_Num[i]));

            readev2.read(reinterpret_cast<char*>(&ADC_Channel[i]),sizeof(ADC_Channel[i]));

        }

        readev2.read( &temp, sizeof(temp));

        delete [] ADC_Num;
        delete [] ADC_Channel;
    }



//---------------------------------------------------------------------------------------Function
//
//  This function will let user to start the appointed start event index.
//
void  skipUnwant(int event_start, fstream &readev2)
{
    char x; // serve as the first ch,
            // it works as ADC_fired_amount variable
            // as read from .ev2 data....for conversion date type.
    unsigned char y;

    int ADC_fired_amount;



    for(int i=0; i < event_start; i++)
    {

        readev2.read( &x, sizeof(x)); //Totally, how many ADCs fire

        y= static_cast<unsigned char>(x);

        ADC_fired_amount = static_cast<int>(y);
        char*  ADC_Num     = new char  [ADC_fired_amount];
        short* ADC_Channel = new short [ADC_fired_amount];

        for (int j=0; j < ADC_fired_amount; j++)
        {
            readev2.read( &ADC_Num[j], sizeof(ADC_Num[j]));

            readev2.read(reinterpret_cast<char*>(&ADC_Channel[j]),sizeof(ADC_Channel[j]));

        }
        delete [] ADC_Num;
        delete [] ADC_Channel;
        readev2.read( &x, sizeof(x));



    }

}


//---------------------------------------------------------------------------------------Function
//
//  Showing the wanted event information.
//
void  showingWanted(int event_start, int N_Event_Set, fstream &readev2)
{
    char x; // serve as the first ch,
            // it works as ADC_fired_amount variable
            // as read from .ev2 data....for conversion date type.

    unsigned char y;

    int ADC_fired_amount;

    bool bad_ADC_Fired = false;



    for(int i = 0; i < N_Event_Set; i++)
    {
        readev2.read( &x, sizeof(x)); //Totally, how many ADCs fire

        y= static_cast<unsigned char>(x);

        ADC_fired_amount = static_cast<int>(y);
        char*  ADC_Num     = new char  [ADC_fired_amount];
        short* ADC_Channel = new short [ADC_fired_amount];

        // since we may have bad event in ev2, so we need to use if statment.
        if (x > 0 && x <= 10)
        {

            cout <<"no."<< event_start+1   // ps. human read index
                 <<"["<<ADC_fired_amount <<"] ";
        }
        else
        {
            //cout <<"|----------------"<< "no." << event_start+1  <<"------------------------|"<<endl;

            cout <<"no."<< event_start+1   // ps. human read index
                 <<"["<<ADC_fired_amount <<"] ";

            bad_ADC_Fired = true;
        }



        bool bad_event = false;
        bool bad_ADC_Num = false;
        bool bad_ADC_Ch_1 = false;
        bool bad_ADC_Ch_2 = false;

        int check_ADC_num;
        int check_ADC_chan;

        for (int j=0; j < ADC_fired_amount; j++)
        {
            readev2.read( &ADC_Num[j], sizeof(ADC_Num[j]));

            check_ADC_num = static_cast<int>(ADC_Num[j]);

            if( check_ADC_num == 0 ) { bad_event = true; bad_ADC_Num = true;}



            readev2.read(reinterpret_cast<char*>(&ADC_Channel[j]),sizeof(ADC_Channel[j]));

            check_ADC_chan = static_cast<int>(ADC_Channel[j]);

            if (ADC_Channel[j] < 0 || ADC_Channel[j] > 8192 )
            {
                bad_event = true;
                if ( ADC_Channel[j] < 0)    { bad_ADC_Ch_1 = true; }
                if ( ADC_Channel[j] > 8192) { bad_ADC_Ch_2 = true; }
            }

        }

        if ( !bad_event ) // if without bad event.
        {
            for(int j = 0; j < ADC_fired_amount; j++)
            {

                if( ADC_fired_amount <= 8 )
                {
                    if( static_cast<int>(ADC_Num[j]) > 0 )
                    {
                        cout <<"(#:" <<static_cast<int>(ADC_Num[j]);
                    }

                    if ( static_cast<int>(ADC_Num[j]) < 0)
                    {
                        cout <<"(#:" <<static_cast<int>(ADC_Num[j]) + 256;
                    }

                    cout<<"  Ch:"<< ADC_Channel[j]<<")  ";

                }

                 if( ADC_fired_amount > 8 )
                 {
                    if( static_cast<int>(ADC_Num[j]) > 0 )
                    {
                        cout <<"\n(#:" <<static_cast<int>(ADC_Num[j]);
                    }

                    if ( static_cast<int>(ADC_Num[j]) < 0)
                    {
                        cout <<"(#:" <<static_cast<int>(ADC_Num[j]) + 256;
                    }

                    cout<<"  Ch:"<< ADC_Channel[j]<<")";

                 }


            }


        }else if( bad_event )
        {
            cout << "bad events: ";
            if(bad_ADC_Num )                         { cout << " ADC_Num =0\t";}
            if(bad_ADC_Ch_1 && !bad_ADC_Num )        { cout << " ADC_ch < 0 \t";}
            if(bad_ADC_Ch_2 && !bad_ADC_Num )        { cout << " ADC_ch > 8192";}
        }


        cout <<"\n\n";

        delete [] ADC_Num;
        delete [] ADC_Channel;
        readev2.read( &x, sizeof(x));

        event_start++;

    }
}


void  __showVersion()
{
    cout << "\n Readev2 by Pei-Luan Tai" << " Lastest updated "<<"May. 09 2015\n\n";
}
