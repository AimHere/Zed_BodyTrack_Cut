#pragma once

#include <array>
#include <string>
//#include <utility.h>
//#include <nlohmann/json.hpp>


// Max Skeletons should be determined at runtime from a config file
//constexpr int MAX_SKELETONS = 10;

constexpr int NUM_BONES = 34;

#define BODY_PART_TEST static_cast<int>(BODY_PARTS_POSE_34::RIGHT_ELBOW)

namespace zed {

    struct vec3 { float x, y, z; };
    struct quat { float x, y, z, w; };
    struct xform { vec3 pos; quat ori; };


    struct UnityZedData
    {
        int numBodies;
        double elapsed;
        quat skeletonData[NUM_BONES * 10];
        int skeletonID[10];
        xform rootTransform[10];
    };


    struct ZedSkeletonData
    {
        // skeleton id
        int id;
        // root transform
        xform root_transform;
        // bones: 1 quaternion per bone
        std::array<quat, NUM_BONES> bone_data;
    };


    // Packed into one struct to make constructing the IPC easier

    // Because of the dynamic size allocation, please note that the sizeof
    // operator won't work properly; if you want the size of a particular instance,
    // then call the size method on that instance.

    // If you don't have a populated instance, then call ZedBodies::size(numBodies) 
    // with how many skeletons

    struct ZedBodies
    {
        int num_skeletons = 0;
        double elapsed = 0.0; // Elapsed timestamp
        ZedSkeletonData skeletons[0];

        size_t size() {
            return (sizeof(ZedBodies) + num_skeletons * sizeof(ZedSkeletonData));
        }
        static size_t size(int numskels) {
            return (sizeof(ZedBodies) + numskels * sizeof(ZedSkeletonData));
        }

        // Ugh. Reinventing new 
        static ZedBodies* allocate(int numskels)
        {
            ZedBodies* zp = (ZedBodies*)malloc(ZedBodies::size(numskels));
            return zp;
        }

    };

    // We probably want to use long ints as the timestamp going onwards
    struct ZedBodiesLegacy
    {
        int num_skeletons = 0;
        double elapsed = 0.0; // Elapsed timestamp
        ZedSkeletonData skeletons[0];

        size_t size() {
            return (sizeof(ZedBodiesLegacy) + num_skeletons * sizeof(ZedSkeletonData));
        }
        static size_t size(int numskels) {
            return (sizeof(ZedBodiesLegacy) + numskels * sizeof(ZedSkeletonData));
        }

        // Ugh. Reinventing new 
        static ZedBodiesLegacy* allocate(int numskels)
        {
            ZedBodiesLegacy* zp = (ZedBodiesLegacy*)malloc(ZedBodiesLegacy::size(numskels));
            return zp;
        }

    };


	// Body position indices ripped straight from Camera.hpp in the ZED SDK. 

    enum class BODY_PARTS_POSE_34
    {
        PELVIS = 0,
        NAVAL_SPINE = 1,
        CHEST_SPINE = 2,
        NECK = 3,
        LEFT_CLAVICLE = 4,
        LEFT_SHOULDER = 5,
        LEFT_ELBOW = 6,
        LEFT_WRIST = 7,
        LEFT_HAND = 8,
        LEFT_HANDTIP = 9,
        LEFT_THUMB = 10,
        RIGHT_CLAVICLE = 11,
        RIGHT_SHOULDER = 12,
        RIGHT_ELBOW = 13,
        RIGHT_WRIST = 14,
        RIGHT_HAND = 15,
        RIGHT_HANDTIP = 16,
        RIGHT_THUMB = 17,
        LEFT_HIP = 18,
        LEFT_KNEE = 19,
        LEFT_ANKLE = 20,
        LEFT_FOOT = 21,
        RIGHT_HIP = 22,
        RIGHT_KNEE = 23,
        RIGHT_ANKLE = 24,
        RIGHT_FOOT = 25,
        HEAD = 26,
        NOSE = 27,
        LEFT_EYE = 28,
        LEFT_EAR = 29,
        RIGHT_EYE = 30,
        RIGHT_EAR = 31,
        LEFT_HEEL = 32,
        RIGHT_HEEL = 33,

        LAST = 34

    };


    class BufferProperties {
    public:
        // Whether this has been populated already; needed because there doesn't seem to be an 'init' function or similar for the config dll
        bool populated = false;

        int maxBodyCount; // Maximum number of bodies the zedcam will see. If more people are tracked, it takes the oldest ones. Should probably be 1 or 2
        std::string name; // Name of the ringbuffer
        int numEntries; // Number of copies of the bufferlist are in the ringbuffer; Suggested value 3

        float writeRate; // (provisional) how often the buffer is written to, in fps
        float readRate; // (provisional) how often the buffer is read from, in fps
        
        float confidence; //Object detection confidence threshold

        int fps; // Really we'd prefer this to be a string, but it'll confuse things when writing the config file
        std::string depth;
        std::string resolution;
        std::string coordsystem;

        std::string videoFile;
        std::string trackRecording; // TODO: Get a better interface for this

        /*
        void populate_from_json(std::string filename) {
            auto jsonFilename = filename;
            auto jText = dancenet::readTextFile(jsonFilename);

            auto j = json::parse(jText);

            j.at("zedMaxBodyCount").get_to(maxBodyCount);
            j.at("zedBufferName").get_to(name);
            j.at("zedBufferEntries").get_to(numEntries);
            j.at("zedBufferWriteRate").get_to(writeRate);
            j.at("zedBufferReadRate").get_to(readRate);
            j.at("zedFPS").get_to(fps);
            j.at("zedDepth").get_to(depth);
            j.at("zedResolution").get_to(resolution);
            j.at("zedConfidenceThreshold").get_to(confidence);
            j.at("zedCoordinateSystem").get_to(coordsystem);
            j.at("zedRecordVideo").get_to(videoFile);
            j.at("zedTrackRecording").get_to(trackRecording);
            populated = true;
        }
        */

        void debug_defaults() {
            maxBodyCount = 10;
            name = "ZED_Test";
            numEntries = 3;
            writeRate = 30.0;
            readRate = 30.0;
            fps = 30;
            depth = "ULTRA";
            resolution = "1080";
            confidence = 40.0;
            populated = true;
            coordsystem = "RIGHT_HANDED_Z_UP_X_FWD";
            videoFile = "";
        }
        /*
        void populate_from_json() {
            // TODO : Fix this hardcoded __FILE__ nonsense!
           populate_from_json(std::filesystem::path(__FILE__).parent_path().string() + "\\sensorConfig.json");
           //populate_from_json("../sensorConfig.json");
        }
        */
    };


}