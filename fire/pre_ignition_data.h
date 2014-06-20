#ifndef PREIGNITIONDATA_H
#define PREIGNITIONDATA_H

class PreIgnitionData {
  public:
    enum Type{PREHEATING, INCIDENT, IGNITION};

    static std::string getTypeLabel(Type type) {
      switch(type) {
        case Type::PREHEATING: return "pre-heating";
        case Type::INCIDENT:   return "incident   ";
        case Type::IGNITION:   return "ignition   ";
        default: throw std::runtime_error("unknown type");
      }
    }

    // static factory methods to create data objects
    static PreIgnitionData preheating(double dryingFactor, double temperature, double duration) {
      PreIgnitionData rec;
      rec.type_ = Type::PREHEATING;
      rec.dryingFactor_ = dryingFactor;
      rec.temperature_ = temperature;
      rec.duration_ = duration;
      return rec;
    }
    
    static PreIgnitionData incident(double dryingFactor, double temperature) {
      PreIgnitionData rec;
      rec.type_ = Type::INCIDENT;
      rec.dryingFactor_ = dryingFactor;
      rec.temperature_ = temperature;
      return rec;
    }
    
    static PreIgnitionData ignition(double dryingFactor, double temperature) {
      PreIgnitionData rec;
      rec.type_ = Type::IGNITION;
      rec.dryingFactor_ = dryingFactor;
      rec.temperature_ = temperature;
      return rec;
    }

    // accessors
    Type type() { return type_; }
    
    double dryingFactor() { return dryingFactor_; }
    
    double temperature() { return temperature_; }
    
    double duration() {
      if (type_ != Type::PREHEATING) 
        throw std::runtime_error("duration only defined for preheating data");

      return duration_;
    }

  private:
    // private constructor
    PreIgnitionData() {};

    Type type_;
    double dryingFactor_;
    double temperature_;
    double duration_;
};


#endif
