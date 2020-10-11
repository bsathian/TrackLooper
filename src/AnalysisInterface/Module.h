#ifndef ANALYSIS_INTERFACE_H
#define ANALYSIS_INTERFACE_H

#include <vector>
#include <map>
#include <tuple>

#include "Hit.h"
#include "MiniDoublet.h"
#include "Segment.h"
#include "Tracklet.h"

namespace SDL
{
    class Hit;
    class MiniDoublet;
    class Segment;
    class Tracklet;

    class Module
    {
        private:
            unsigned int detId_;
            short layer_;
            short ring_;
            short rod_;
            short module_;
            bool isInverted_;
            bool isLower_;
        public:
            enum SubDet
            {
                Barrel = 5,
                Endcap = 4
            };
        private:
            SubDet subdet_;
        public:
            enum ModuleType
            {
                PS,
                TwoS
            };
        private:

            ModuleType moduleType_;
        public:
            enum ModuleLayerType
            {
                Pixel,
                Strip
            };
        private:
            ModuleLayerType moduleLayerType_;
        public:
            enum Side
            {
                NegZ = 1,
                PosZ = 2,
                Center = 3
            };
        private:
            Side side_;

            std::vector<Hit*> hits_;
            std::vector<MiniDoublet*> miniDoublets_;
            std::vector<Segment*> segments_;
            std::vector<Tracklet*> tracklets_;

        public:
            Module();
            Module(unsigned int detId);
            Module(unsigned int detId, short layer, short ring, short rod, short module, bool isInverted, bool isLower, SubDet subdet, ModuleType moduleType, short side);
            ~Module();

            // accessor functions
            const unsigned int& detId() const;
            const unsigned int& partnerDetId() const;
            const short& subdet() const;
            const short& side() const;
            const short& layer() const;
            const short& rod() const;
            const short& ring() const;
            const short& module() const;
            const bool& isLower() const;
            const bool& isInverted() const;
            const ModuleType& moduleType() const;
            const ModuleLayerType& moduleLayerType() const;
            const std::vector<Hit*>& getHitPtrs() const;
            const std::vector<MiniDoublet*>& getMiniDoubletPtrs() const;
            const std::vector<Segment*>& getSegmentPtrs() const;
            const std::vector<Tracklet*>& getTrackletPtrs() const;

            // static functions to parse detId
            static unsigned short parseSubdet(unsigned int);
            static unsigned short parseSide(unsigned int);
            static unsigned short parseLayer(unsigned int);
            static unsigned short parseRod(unsigned int);
            static unsigned short parseRing(unsigned int);
            static unsigned short parseModule(unsigned int);
            static unsigned short parseIsLower(unsigned int);
            static bool parseIsInverted(unsigned int);
            static unsigned int parsePartnerDetId(unsigned int);
            static ModuleType parseModuleType(unsigned int);
            static ModuleLayerType parseModuleLayerType(unsigned int);
            void setDerivedQuantities();


            void addHit(Hit* hit);
            void addMiniDoublet(MiniDoublet* md);
            void addSegment(Segment* sg);
            void addTracklet(Tracklet* tp);
    };

}
#endif
