#ifndef PREIGNITIONDATA_H
#define PREIGNITIONDATA_H

#include "flame.h"
#include "pre_heating_flame.h"

class PreIgnitionData {
  public:
    enum Type{PREHEATING, INCIDENT};

    static std::string getTypeLabel(Type type) {
      switch(type) {
        case Type::PREHEATING: return "pre-heating";
        case Type::INCIDENT:   return "incident";
        default: throw std::runtime_error("unknown type");
      }
    }

    // static factory methods to create data objects
    static PreIgnitionData preheating(double flameLength, double depthIgnited, double distanceToFlame, 
                                      double dryingFactor, double temperature, double duration) {
      PreIgnitionData rec;
      rec.type_ = Type::PREHEATING;
      rec.flameLength_ = flameLength;
      rec.depthIgnited_ = depthIgnited;
      rec.distanceToFlame_ = distanceToFlame;
      rec.dryingFactor_ = dryingFactor;
      rec.temperature_ = temperature;
      rec.duration_ = duration;
      return rec;
    }
    
    static PreIgnitionData incident(double flameLength, double depthIgnited, double distanceToFlame,
                                    double dryingFactor, double temperature, double idt) {
      PreIgnitionData rec;
      rec.type_ = Type::INCIDENT;
      rec.flameLength_ = flameLength;
      rec.depthIgnited_ = depthIgnited;
      rec.distanceToFlame_ = distanceToFlame;
      rec.dryingFactor_ = dryingFactor;
      rec.temperature_ = temperature;
      rec.idt_ = idt;
      return rec;
    }
    
    // accessors
    Type type() const { return type_; }

    double flameLength() const { return flameLength_; }

    double depthIgnited() const { return depthIgnited_; }

    double distanceToFlame() const { return distanceToFlame_; }
    
    double dryingFactor() const { return dryingFactor_; }
    
    double temperature() const { return temperature_; }
    
    double duration() const {
      if (type_ != Type::PREHEATING) 
        throw std::runtime_error("duration only defined for preheating data");

      return duration_;
    }

    double idt() const {
      if (type_ == Type::PREHEATING) 
        throw std::runtime_error("idt only defined for incident and ignition data");

      return idt_;
    }

  private:
    // private constructor
    PreIgnitionData() {};

    Type type_;

    double flameLength_;
    double depthIgnited_;
    double distanceToFlame_;

    double dryingFactor_;
    double temperature_;
    double duration_;
    double idt_;
};


#endif
