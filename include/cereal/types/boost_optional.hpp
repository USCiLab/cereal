#ifndef CEREAL_TYPES_BOOST_OPTIONAL_
#define CEREAL_TYPES_BOOST_OPTIONAL_

#include <cereal/cereal.hpp>
#include <boost/optional.hpp>

namespace cereal {
    template <class Archive, typename T> inline
        void save(Archive& ar, const boost::optional<T> optional) {
            bool initialized = optional.is_initialized();
    
            ar(CEREAL_NVP_("initialized", initialized));
            if(!initialized) return;
            
            bool none = optional == boost::none;
            if(none) {
                ar(CEREAL_NVP_("none", true));
                return;
            }
            
            ar(CEREAL_NVP_("none", false), CEREAL_NVP_("value", *optional));
        }
        
    template <class Archive, typename T> inline
        void load(Archive& ar, boost::optional<T>& optional) {
            bool initialized;
            ar(CEREAL_NVP_("initialized", initialized));
           
            if(!initialized) return;
                
            bool none;
            ar(CEREAL_NVP_("none", none));
            
            if(none) {
                optional = boost::none;
            } else {
                T value;
                ar(CEREAL_NVP_("value", value));
                optional = value;
            }
        }
        
}

#endif // CEREAL_TYPES_BOOST_OPTIONAL_