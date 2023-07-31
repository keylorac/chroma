
// -*- C++ -*-
/*! \file
 * \brief Inline construction of a sum of propagators
 *
 * Sum of  props
 */
#ifndef __inline_sumprops_h__ /*! Cambiamos los __inline_npr_vertex_c_h__ por __inline_sumprops_h__ */
#define __inline_sumprops_h__

#include "chromabase.h"
#include "meas/inline/abs_inline_measurement.h"
#include "io/qprop_io.h"

namespace Chroma 
{ 
  /*! \ingroup inlinehadron */
  namespace InlineSum_PropsEnv 
  {
    extern const std::string name;
    bool registerAll();
  }


  //! Parameter structure
  /*! \ingroup inlinehadron */
  struct InlineSum_PropsParams 
  {
    InlineSum_PropsParams();
    InlineSum_PropsParams(XMLReader& xml_in, const std::string& path);
    void write(XMLWriter& xml_out, const std::string& path);

    unsigned long frequency;   

    //! Parameters
    struct Param_t
    {
      
      float cons_1;          /*!< constante 2 */
      float cons_2;          /*!< constante 1 */ 
      GroupXML_t   cfs;                /*!< Fermion state */                     
    } param;

    //! Propagators
    struct NamedObject_t
    {
      std::string       gauge_id;        /*!< Input Gauge id */
      std::string       prop_1;         /*!< Input forward props */
      std::string       prop_2;
      std::string	prop_r;
    } named_obj;

    std::string xml_file;  // Alternate XML file pattern
  };


  //! Inline measurement of NPR vertices
  /*! \ingroup inlinehadron */
  class InlineSum_props : public AbsInlineMeasurement 
  {
  public:
    ~InlineSum_props() {}
    InlineSum_props(const InlineSum_PropsParams& p) : params(p) {}
    InlineSum_props(const InlineSum_props& p) : params(p.params) {}

    unsigned long getFrequency(void) const {return params.frequency;}

    

    //! Do the measurement
    void operator()(const unsigned long update_no,
		    XMLWriter& xml_out); 

  protected:
    //! Do the measurement
    void func(const unsigned long update_no,
	      XMLWriter& xml_out); 

  private:
    InlineSum_PropsParams params;
  };

}


#endif





