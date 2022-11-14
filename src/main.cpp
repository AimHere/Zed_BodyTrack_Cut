
/*************************/
// Takes two frame numbers and scrapes a .dat file based on the timestamps in a corresponding svo file
/************************/

// ZED include
#include <sl/Camera.hpp>

// Sample includes
//#include <opencv2/opencv.hpp>
#include "utils.hpp"

#include <fstream>
#include <iostream>

#include <string>
#include <sstream>

#include <stdlib.h>

#include "zed_common.h"
#include <memory.h>

// Using namespace
using namespace sl;
using namespace std;
using namespace zed;


BufferProperties bProperties = BufferProperties();

void print(string msg_prefix, ERROR_CODE err_code = ERROR_CODE::SUCCESS, string msg_suffix = "");
ZedBodiesLegacy* inputTrack;


bool getTrack(std::ifstream & infile, uint64_t timestamp) {

    do {        
        infile.read((char*)inputTrack, ZedBodiesLegacy::size(bProperties.maxBodyCount));
        //cout << "Checking " << timestamp << " against " << (uint64_t)inputTrack->elapsed << "\n";
        if ((uint64_t)inputTrack->elapsed >= timestamp)
        {
            //cout << "Got track with elapsed " << (uint64_t) inputTrack->elapsed << "\n";
            return true;
        }
    } while (infile);
    return false;
}

int main(int argc, char** argv) {

    bProperties.debug_defaults();

    ZedBodiesLegacy* zedBodies = ZedBodiesLegacy::allocate(bProperties.maxBodyCount);

    string inputsvo, inputdat;
    string outfile;    
    int startframe, endframe;


    if (argc == 5) {
        inputsvo = string(argv[1]);
        inputdat = string(argv[2]);
        startframe = atoi(argv[3]);
        endframe = atoi(argv[4]);

        stringstream ss;

        string idstr = string(inputdat);
        ss << idstr.substr(0, idstr.find_last_of("."));
        ss << "_" << startframe << "_" << endframe << ".dat";

        outfile = ss.str();
        cout << "Outputting default outfile: " << outfile << "\n";
    }
    else if (argc == 6) {
        inputsvo = string(argv[1]);
        inputdat = string(argv[2]);
        startframe = atoi(argv[3]);
        endframe = atoi(argv[4]);
        outfile = string(argv[5]);

    }
    else {
        cout << "Usage: \n";
        cout << "$ " << argv[0] << " SVO_file DAT_file start_frame end_frame <output_file>\n";

        free(zedBodies);
        return EXIT_FAILURE;
    }

    // Create ZED objects
    Camera zed;
    InitParameters init_parameters;
    init_parameters.input.setFromSVOFile(inputsvo.c_str());
    init_parameters.depth_mode = sl::DEPTH_MODE::PERFORMANCE;

    // Open the camera
    auto returned_state = zed.open(init_parameters);
    if (returned_state != ERROR_CODE::SUCCESS) {
        print("Camera Open", returned_state, "Exit program.");

        free(zedBodies);
        return EXIT_FAILURE;
    }

    auto resolution = zed.getCameraInformation().camera_configuration.resolution;

    int nb_frames = zed.getSVONumberOfFrames();
    print("[Info] SVO contains " + to_string(nb_frames) + " frames");
    cout << "Body size is " << bProperties.maxBodyCount << ", size is " << ZedBodiesLegacy::size(bProperties.maxBodyCount) << "\n";


    cout << "About to open " << inputdat << "\n";
    ifstream datFile(inputdat, ios::binary | ios::in);
    cout << "Dat file has been opened\n";
    // Start SVO playback

    int framecounter = 0;
    int datcounter = 0;
    int wrotefiles = 0;

    cout << "About to open the output file\n";
    ofstream outputFP(outfile, ios::binary| ios::out);
    cout << "About to allocate for ZedBodies\n";
    inputTrack = (ZedBodiesLegacy *) ZedBodiesLegacy::allocate(ZedBodiesLegacy::size(bProperties.maxBodyCount));

    cout << "InputTrack allocated\n";
    
    while (framecounter <= endframe) {
        

        returned_state = zed.grab();
        if (returned_state != ERROR_CODE::SUCCESS) {
            cout << "Grab error: " << returned_state << "\n";
            break;
        }
        
        auto ts = zed.getTimestamp(TIME_REFERENCE::IMAGE);
        uint64_t us = ts.getMicroseconds();

        if (framecounter >= startframe && framecounter <= endframe) {
            if(!getTrack(datFile, us))
            {      
                
                cout << "Aborted due to dat file overrun\n";
                break;
            }
//            cout << "Going to write track with elapsed " << (uint64_t) inputTrack->elapsed << "\n";
            outputFP.write((char *)inputTrack, ZedBodiesLegacy::size(bProperties.maxBodyCount));
            wrotefiles += 1;
        }
        framecounter += 1;
    }
    cout << "Written " << wrotefiles << " frames to " << outfile << "\n";
    outputFP.close();
    datFile.close();
    zed.close();
    free(inputTrack);
    return EXIT_SUCCESS;
}

void print(string msg_prefix, ERROR_CODE err_code, string msg_suffix) {
    cout <<"[Sample]";
    if (err_code != ERROR_CODE::SUCCESS)
        cout << "[Error] ";
    else
        cout<<" ";
    cout << msg_prefix << " ";
    if (err_code != ERROR_CODE::SUCCESS) {
        cout << " | " << toString(err_code) << " : ";
        cout << toVerbose(err_code);
    }
    if (!msg_suffix.empty())
        cout << " " << msg_suffix;
    cout << endl;
}
