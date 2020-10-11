#ifndef ANALYSIS_INTERFACE_HIT_H
#define ANALYSIS_INTERFACE_HIT_H

#include <vector>
#include <map>
#include <tuple>

#include "Module.h"

namespace SDL
{
    class Module;
    class Hit
    {  
        private:
            float x_;
            float y_;
            float z_;
            float phi_;
            float rt_;
            Hit* hit_high_edge_;
            Hit* hit_low_edge_;
            const Module* modulePtr_;

        public:
            Hit(float x, float y, float z, float phi, float rt, const Module* modulePtr);
            ~Hit();
            // accessor functions
            const float& x() const;
            const float& y() const;
            const float& z() const;
            const float& rt() const;
            const float& phi() const;
            const Module& getModule() const;

            // Set the boundary hits where the hits are shifted
            const Hit* getHitHighEdgePtr() const;
            const Hit* getHitLowEdgePtr() const;
            void setHighEdgePtr(Hit* hitHighEdge);
            void setLowEdgePtr(Hit* hitLowEdge);
    };

}
#endif
