
// -*- C++ -*-
/*! \file
 * \brief Inline of hutchinson trace estimator
	 *
	 * Hutchinson Trace Estimator 
	 */
#ifndef __inline_hutchinson_trace_estimator_h__ /*! Cambiamos los __inline_npr_vertex_c_h__ por __inline_hutchinson_trace_estimator_h__ */
#define __inline_hutchinson_trace_estimator_h__

#include "chromabase.h"
#include "meas/inline/abs_inline_measurement.h"
#include "io/qprop_io.h"

namespace Chroma 
{
  /*! \ingroup inlinehadron */
  namespace InlineHutchinsonTraceEstimatorEnv 
  {
    extern const std::string name;
    bool registerAll();
  }

  //! Parameter structure
  /*! \ingroup inlinehadron */
  struct InlineHutchinsonTraceEstimatorParams 
  {
    InlineHutchinsonTraceEstimatorParams();
    InlineHutchinsonTraceEstimatorParams(XMLReader& xml_in, const std::string& path);
    void write(XMLWriter& xml_out, const std::string& path);

    unsigned long frequency;   

    //! Parameters
    struct Param_t
    {
      
      float cons_1;          /*!< constante 2 */
      float cons_2;          /*!< constante 1 */ 
      GroupXML_t   cfs;                /*!< Fermion state */                     
      int   links_max;
      std::string  file_name;
      //int cfg;
    } param;

    //! Propagators
    struct NamedObject_t
    {
      std::string       gauge_id;        /*!< Input Gauge id */
      std::string       prop_1;         /*!< Input forward props */
      std::string       prop_2;
      std::string	    prop_r;
      std::string       vect_1;
      std::string       vect_2;
      std::string       vect_r;
    } named_obj;

    std::string xml_file;  // Alternate XML file pattern
  };


  //! Inline measurement of HutchinsonTraceEstimator
  /*! \ingroup inlinehadron */
  class InlineHutchinson_Trace_Estimator : public AbsInlineMeasurement 
  {
  public:
    ~InlineHutchinson_Trace_Estimator() {}
    InlineHutchinson_Trace_Estimator(const InlineHutchinsonTraceEstimatorParams& p) : params(p) {}
    InlineHutchinson_Trace_Estimator(const InlineHutchinson_Trace_Estimator& p) : params(p.params) {}

    unsigned long getFrequency(void) const {return params.frequency;}

    

    //! Do the measurement
    void operator()(const unsigned long update_no,
		    XMLWriter& xml_out); 

  protected:
    //! Do the measurement
    void func(const unsigned long update_no,
	      XMLWriter& xml_out); 

  private:
    InlineHutchinsonTraceEstimatorParams params;
  };

}


#endif





