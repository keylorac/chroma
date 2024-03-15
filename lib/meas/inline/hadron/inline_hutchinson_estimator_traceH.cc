/*! \file
 * \brief Inline construction of SUM of props
 *
 * SUM of props
 */
#include <cmath>
#include "meas/inline/hadron/inline_hutchinson_estimator_trace.h"
#include "meas/inline/abs_inline_measurement_factory.h"
#include "meas/glue/mesplq.h"
#include "util/ft/sftmom.h"
// #include "meas/hadron/BuildingBlocks_w.h"
#include "util/info/proginfo.h"
#include "meas/inline/make_xml_file.h"

#include "meas/inline/io/named_objmap.h"
#include "actions/ferm/fermstates/ferm_createstate_factory_w.h"
#include "actions/ferm/fermstates/ferm_createstate_aggregate_w.h"

#include "actions/ferm/fermacts/fermact_factory_w.h"
#include "actions/ferm/fermacts/fermacts_aggregate_w.h"

#include "eoprec_logdet_wilstype_fermact_w.h"
#include "actions/ferm/linop/lgherm_w.h"

#include "actions/ferm/fermacts/clover_fermact_params_w.h"
#include "actions/ferm/fermacts/wilson_fermact_params_w.h"
#include "actions/ferm/linop/linop_w.h"


  void z2_src_t(QDP::LatticeFermion & a)
  {

    a = zero ;
    LatticeReal rnd ;
    LatticeReal ar ,ai ;
    LatticeColorVector colorvec = zero;

    for(int spin_index= 0 ; spin_index < Ns ; ++spin_index)
      for(int color_index= 0 ; color_index < Nc ; ++color_index)
      {
    random(rnd) ;
    ar = where( rnd > 0.5 , LatticeReal(1) , LatticeReal(-1) );
    random(rnd) ;
    ai = where( rnd > 0.5 , LatticeReal(1) , LatticeReal(-1) );
    LatticeComplex c = cmplx(ar,0) ;

    colorvec = peekSpin(a,spin_index);

    pokeSpin(a,pokeColor(colorvec,c,color_index),spin_index);
      }

  }

namespace Chroma 
{ 
  namespace InlineHutchinsonTraceEstimatorEnv 
  { 
    namespace
    {
      AbsInlineMeasurement* createMeasurement(XMLReader& xml_in, 
					      const std::string& path) 
      {
	return new InlineHutchinson_Trace_Estimator(InlineHutchinsonTraceEstimatorParams(xml_in, path));
      }
      //! Local registration flag
      bool registered = false;
    }
    const std::string name = "HUTCHINSON_ESTIMATOR_TRACE";
    //! Register all the factories
    bool registerAll() 
    {
      bool success = true; 
      if (! registered)
      {
	success &= CreateFermStateEnv::registerAll();
    success &= WilsonTypeFermActsEnv::registerAll();
	success &= TheInlineMeasurementFactory::Instance().registerObject(name, createMeasurement);
	registered = true;
      }
      return success;
    }
  }
  
  typedef LatticeFermion               T;
  typedef multi1d<LatticeColorMatrix>  P; 
  typedef multi1d<LatticeColorMatrix>  Q;
  //! Param input
  void read(XMLReader& xml, const std::string& path, InlineHutchinsonTraceEstimatorParams::Param_t& input)
  {
    XMLReader paramtop(xml, path);
    int version;
    read(paramtop, "version", version);
    
    if (paramtop.count("FermState") != 0)
      input.cfs = readXMLGroup(paramtop, "FermState", "Name");
    else
      input.cfs = CreateFermStateEnv::nullXMLGroup();
    
    switch (version) 
    {
    case 1:
      break;
    default :
      QDPIO::cerr << InlineHutchinsonTraceEstimatorEnv::name << ": input parameter version " 
		  << version << " unsupported." << std::endl;
      QDP_abort(1);
    }
    
      read(paramtop, "cons_1", input.cons_1);
      read(paramtop, "cons_2", input.cons_2);

  }
  //! Param write
  void write(XMLWriter& xml, const std::string& path, const InlineHutchinsonTraceEstimatorParams::Param_t& input)
  {
    push(xml, path);
    int version = 1;
    write(xml, "version", version);
    //write(xml, "links_max", input.links_max);
    write(xml, "cons_1", input.cons_1);
    write(xml, "cons_1", input.cons_2);    
    xml << input.cfs.xml;
    pop(xml);
  }
  //! Propagator input
  void read(XMLReader& xml, const std::string& path, InlineHutchinsonTraceEstimatorParams::NamedObject_t& input)
  {
    XMLReader inputtop(xml, path);
    read(inputtop, "gauge_id", input.gauge_id);
    read(inputtop, "prop_1", input.prop_1); //Agregamos el id del propagador 1
    read(inputtop, "prop_2", input.prop_2); //Agregamos el id del propagador 2
    read(inputtop, "prop_r", input.prop_r); //Agregamos el id del propagador resultante
  }
  //! Propagator output
  void write(XMLWriter& xml, const std::string& path, const InlineHutchinsonTraceEstimatorParams::NamedObject_t& input)
  {
    push(xml, path);
    write(xml, "gauge_id", input.gauge_id);
    write(xml, "prop_1", input.prop_1);
    write(xml, "prop_2", input.prop_2);
    write(xml, "prop_r", input.prop_r);
    pop(xml);
  }
  // Param stuff
  InlineHutchinsonTraceEstimatorParams::InlineHutchinsonTraceEstimatorParams() {frequency = 0;}
  InlineHutchinsonTraceEstimatorParams::InlineHutchinsonTraceEstimatorParams(XMLReader& xml_in, const std::string& path) 
  {
    try 
    {
      XMLReader paramtop(xml_in, path);
      if (paramtop.count("Frequency") == 1)
	read(paramtop, "Frequency", frequency);
      else
	frequency = 1;
      // Read program parameters
      read(paramtop, "Param", param);
      // Read in the output propagator/source configuration info
      read(paramtop, "NamedObject", named_obj);
      // Possible alternate XML file pattern
      if (paramtop.count("xml_file") != 0) 
      {
	read(paramtop, "xml_file", xml_file);
      }
    }
    catch(const std::string& e) 
    {
      QDPIO::cerr << __func__ << ": Caught Exception reading XML: " << e << std::endl;
      QDP_abort(1);
    }
  }
  void
  InlineHutchinsonTraceEstimatorParams::write(XMLWriter& xml_out, const std::string& path) 
  {
    push(xml_out, path);
    
    Chroma::write(xml_out, "Param", param); 
    Chroma::write(xml_out, "NamedObject", named_obj);
    QDP::write(xml_out, "xml_file", xml_file);
    pop(xml_out);
  }
 
  // Function call
  void 
  InlineHutchinson_Trace_Estimator::operator()(unsigned long update_no,
				   XMLWriter& xml_out) 
  {
    // If xml file not empty, then use alternate
    if (params.xml_file != "")
    {
      std::string xml_file = makeXMLFileName(params.xml_file, update_no);
      push(xml_out, "HUTCHINSON_ESTIMATOR_TRACE");
      write(xml_out, "update_no", update_no);
      write(xml_out, "xml_file", xml_file);
      pop(xml_out);
      XMLFileWriter xml(xml_file);
      func(update_no, xml);
    }
    else
    {
      func(update_no, xml_out);
    }
  }
  // Function call
  void 
  InlineHutchinson_Trace_Estimator::func(unsigned long update_no,
			     XMLWriter& xml_out) 
  {
    START_CODE();
    StopWatch snoop;
    snoop.reset();
    snoop.start();
    push(xml_out, "HUTCHINSON_ESTIMATOR_TRACE");
    write(xml_out, "update_no", update_no);
    //#################################################################################//
    // XML output
    //#################################################################################//
    proginfo(xml_out);    // Print out basic program info
    push(xml_out, "Output_version");
    write(xml_out, "out_version", 2);
    pop(xml_out);
 



    // Test and grab a reference to the gauge field
    XMLBufferWriter gauge_xml;
    try
    {
      TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(params.named_obj.gauge_id);
      TheNamedObjMap::Instance().get(params.named_obj.gauge_id).getRecordXML(gauge_xml);
    }
    catch( std::bad_cast )
    {
      QDPIO::cerr << InlineHutchinsonTraceEstimatorEnv::name << ": caught dynamic cast error"
          << std::endl;
      QDP_abort(1);
    }
    catch (const std::string& e)
    {
      QDPIO::cerr << InlineHutchinsonTraceEstimatorEnv::name << ": std::map call failed: " << e
          << std::endl;
      QDP_abort(1);
    }
    const multi1d<LatticeColorMatrix>& u =
      TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(params.named_obj.gauge_id);

    push(xml_out, "propagator");
    write(xml_out, "update_no", update_no);

    proginfo(xml_out);    // Print out basic program info

    // Write out the input
    //params.writeXML(xml_out, "Input");

    // Write out the config header
    write(xml_out, "Config_info", gauge_xml);

    push(xml_out, "Output_version");
    write(xml_out, "out_version", 1);
    pop(xml_out);


 
    //#################################################################################//
    // Action
    //#################################################################################//

    //
    // Initialize fermion action
    //
    std::istringstream  xml_s(params.param.fermact.xml);
    XMLReader  fermacttop(xml_s);
    QDPIO::cout << "FermAct = " << params.param.fermact.id << std::endl;


    //
    // Try the factories
    //
    bool success = false;
    Handle< LinearOperator<T> > linop;
    if (! success)
    {
      try
      {
	StopWatch swatch;
	swatch.reset();
	QDPIO::cout << "Try the various factories" << std::endl;

	// Typedefs to save typing
	typedef LatticeFermion               T;
	typedef multi1d<LatticeColorMatrix>  P;
	typedef multi1d<LatticeColorMatrix>  Q;

	// Generic Wilson-Type stuff
	//Handle< FermionAction<T,P,Q> >
	//  S_f(TheFermionActionFactory::Instance().createObject(params.param.fermact.id,
	//						       fermacttop,
	//						       params.param.fermact.path));

    Handle< FermAct4D<T,P,Q> > S_f;

    S_f = dynamic_cast<FermAct4D<T,P,Q>*>(TheFermionActionFactory::Instance().createObject(params.param.fermact.id,
                                   fermacttop,
                                   params.param.fermact.path));

	Handle< FermState<T,P,Q> > state(S_f->createState(u));
     
    linop = S_f->linOp(state);
    //Handle< LinearOperator<LatticeFermion> > MM(S_f->lMdagM(state));

	QDPIO::cout << "Suitable factory found: getting Dslash trace" << std::endl;
	swatch.start();
    success = true;
      }
      catch (const std::string& e)
      {
    QDPIO::cout << InlineHutchinsonTraceEstimatorEnv::name << ": caught exception around hutchison estimator action creation: " << e << std::endl;
      }
    }


    //#################################################################################//
    // Construct Building Blocks                                                       //
    //#################################################################################//
    QDP::StopWatch swatch;
    swatch.reset();
    
    XMLBufferWriter file_xml;
    push(file_xml, "HUTCHINSON_ESTIMATOR_TRACE");
    write(file_xml, "Param", params.param);
    pop(file_xml);

    Seed ran_seed;
    QDP::RNG::savern(ran_seed);

    LatticeReal rnd1;
    random(rnd1); //Salva un random en cada punto del Lattice


    //restore the see
    QDP::RNG::setrn(ran_seed);

    LatticeFermion entrada;
    LatticeFermion salida;

    multi1d <LatticeComplex> temp;


    int nvec=10;
    temp.resize(nvec);

       for(int i=0; i<nvec; i++){ // i in range(0,100)
          z2_src_t(entrada);   //crea el z2 noise vector
          salida=entrada;      //copia

          //linop.getFermBC();
          //linop(salida,entrada,PLUS);
          (*linop)(salida,entrada,PLUS);
          //DComplex ip= innerProduct(chi,temp);  

          for(int s=0; s<4;s++){
            for(int c=0; c<3;c++){
              QDPIO::cout << "temp_"<<i<<"(1000,"<<s<<","<<c<<") = " << entrada.elem(1000).elem(s).elem(c).real()<<"+";
              QDPIO::cout << entrada.elem(1000).elem(s).elem(c).imag()<<"i\n";
            }
          }
          temp[i] = innerProduct(entrada,salida); //producto punto
          QDPIO::cout << "temp["<<i<<"]"<<temp[i].elem(1000).elem().elem().real()<<"i\n";
       }
     
      Complex acumulado=0;

      for(int i=0; i<nvec; i++){ 
        acumulado+=sum(temp[i]);
      }
      acumulado/=nvec;

      int entradasDiagonal = QDP::Layout::vol()*12;
      Complex esperado=acumulado/entradasDiagonal; 


      QDPIO::cout << "Traza = " << entradasDiagonal <<"\n";
      QDPIO::cout << "El valor esperado es " << esperado.elem().elem().elem().real()<<"\n";

    pop(xml_out);   // HUTCHINSON_ESTIMATOR_TRACE

    QDPIO::cout << InlineHutchinsonTraceEstimatorEnv::name << ": ran successfully" << std::endl;
    END_CODE();
  } 
}
