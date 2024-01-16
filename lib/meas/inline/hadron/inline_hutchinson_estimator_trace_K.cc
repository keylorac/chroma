/*! \file
 * \brief Inline construction of Hutchinson_trace_estimator
 *
 * Hutchinson_trace_estimator
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
#include "chromabase.h"


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
    //const std::string name = "SET_RNG"; esto por si quisieramos leer y guardar el seed random
    //! Register all the factories

    bool registerAll() 
    {
      bool success = true; 
      if (! registered)
      {
	success &= CreateFermStateEnv::registerAll();
	success &= TheInlineMeasurementFactory::Instance().registerObject(name, createMeasurement);
	registered = true;
      }
      return success;
    }
  }

 
  //! Param input NO APLICA EN ESTE CASO
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
    read(paramtop, "links_max", input.links_max);
    read(paramtop, "file_name", input.file_name);
    //read(paramtop,"cfg",input.cfg);
  }
  //! Param write
  void write(XMLWriter& xml, const std::string& path, const InlineHutchinsonTraceEstimatorParams::Param_t& input)
  {
    push(xml, path);
    int version = 1;
    write(xml, "version", version);
    write(xml, "links_max", input.links_max);
    write(xml, "cons_1", input.cons_1);
    write(xml, "cons_1", input.cons_2);    
    xml << input.cfs.xml;
    pop(xml);
  }
  //! Vector input
  void read(XMLReader& xml, const std::string& path, InlineHutchinsonTraceEstimatorParams::NamedObject_t& input)
  {
    XMLReader inputtop(xml, path);
    read(inputtop, "gauge_id", input.gauge_id);
    //read(inputtop, "vect_1", input.vect_1); //Agregamos el id del vector
    //read(inputtop, "vect_2", input.vect_2); //Agregamos el id del los vector transpuesto
    //read(inputtop, "vect_r", input.vect_r); //Agregamos el id del resultado de la estimacion de la traza resultante
  }
  //! Vector output
  void write(XMLWriter& xml, const std::string& path, const InlineHutchinsonTraceEstimatorParams::NamedObject_t& input)
  {
    push(xml, path);
    write(xml, "gauge_id", input.gauge_id);
    //write(xml, "vect_1", input.vect_1);
    //write(xml, "vect_2", input.vect_2);
    //write(xml, "vect_r", input.vect_r);
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
      //read(paramtop, "Param", param);
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
    
    //Chroma::write(xml_out, "Param", param); 
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
      push(xml_out, "HUTCHINSON_TRACE_ESTIMATOR");
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
			     XMLWriter& XmlOut) 
  {
    START_CODE();
    StopWatch snoop;
    snoop.reset();
    snoop.start();
    push(XmlOut, "HUTCHINSON_TRACE_ESTIMATOR");
    write(XmlOut, "update_no", update_no);
    //#################################################################################//
    // XML output
    //#################################################################################//
    proginfo(XmlOut);    // Print out basic program info
    push(XmlOut, "Output_version");
    write(XmlOut, "out_version", 2);
    pop(XmlOut);
    //###############################################################################//
    // Read Gauge Field                                                              //
    //###############################################################################//
    QDPIO::cout << "Attempt to initialize the gauge field" << std::endl << std::flush ;
    // Grab the gauge field
    multi1d<LatticeColorMatrix> U;
    XMLBufferWriter gauge_xml;
    try
    {
      U = TheNamedObjMap::Instance().getData< multi1d<LatticeColorMatrix> >(params.named_obj.gauge_id);
      TheNamedObjMap::Instance().get(params.named_obj.gauge_id).getRecordXML(gauge_xml);
      // Set the construct state and modify the fields
      {
	QDPIO::cout << "cfs=XX" << params.param.cfs.xml << "XX" << std::endl;
	std::istringstream  xml_s(params.param.cfs.xml);
	XMLReader  fermtop(xml_s);
	Handle<CreateFermState< LatticeFermion, multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> > > 
	  cfs(TheCreateFermStateFactory::Instance().createObject(params.param.cfs.id,
								 fermtop,
								 params.param.cfs.path));
	Handle<FermState< LatticeFermion, multi1d<LatticeColorMatrix>, multi1d<LatticeColorMatrix> > > 
	  state((*cfs)(U));
	// Pull the u fields back out from the state since they might have been
	// munged with fermBC's
	U = state->getLinks();
      }
    
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
    catch( ... )
    {
      QDPIO::cerr << InlineHutchinsonTraceEstimatorEnv::name << ": caught generic exception "
		  << std::endl;
      QDP_abort(1);
    }
    // Write out the input
    params.write(XmlOut, "Input");
    // Write out the config info
    write(XmlOut, "Config_info", gauge_xml);
   
    //#################################################################################//
    // Read Forward Propagator                                                         //
    //#################################################################################//
    SftMom phases_nomom1( 0, true, Nd-1 );  // used to check props. Fix to Nd-1 direction.


    LatticePropagator P1;
    ChromaProp_t prop_header;
    PropSourceConst_t source_header;
    QDPIO::cout << "Attempt to parse forward propagator" << std::endl;
    QDPIO::cout << "parsing forward propagator " << params.named_obj.prop_1 << " ... " << std::endl << std::flush;


    SftMom phases_nomom2( 0, true, Nd-1 );  // used to check props. Fix to Nd-1 direction.

    LatticePropagator P2;
    ChromaProp_t prop_header_1;
    PropSourceConst_t source_header_1;
    QDPIO::cout <<"Attempt to parse forward propagator"<< std::endl; 
    QDPIO::cout <<"parsing forward propagator"<< params.named_obj.prop_2 << "..." << std::endl << std::flush;
 

   
    try
    {
      // Snarf a copy
      P1 = TheNamedObjMap::Instance().getData<LatticePropagator>(params.named_obj.prop_1);
	
      // Snarf the frwd prop info. This is will throw if the frwd prop id is not there
      XMLReader PropXML, PropRecordXML;
      TheNamedObjMap::Instance().get(params.named_obj.prop_1).getFileXML(PropXML);
      TheNamedObjMap::Instance().get(params.named_obj.prop_1).getRecordXML(PropRecordXML);
      // Try to invert this record XML into a ChromaProp struct
      {
	read(PropRecordXML, "/Propagator/ForwardProp", prop_header);
	read(PropRecordXML, "/Propagator/PropSource", source_header);
      }
      // Sanity check - write out the norm2 of the forward prop in the j_decay direction
      // Use this for any possible verification
      {
	multi1d<Double> PropCheck = 
	  sumMulti( localNorm2( P1 ), phases_nomom1.getSet() );
	QDPIO::cout << "forward propagator check = " << PropCheck[0] << std::endl;
	// Write out the forward propagator header
	push(XmlOut, "ForwardProp");
	write(XmlOut, "PropXML", PropXML);
	write(XmlOut, "PropRecordXML", PropRecordXML);
	write(XmlOut, "PropCheck", PropCheck);
	pop(XmlOut);
      }
    }
    catch( std::bad_cast ) 
    {
      QDPIO::cerr << InlineHutchinsonTraceEstimatorEnv::name << ": caught dynamic cast error" 
		  << std::endl;
      QDP_abort(1);
    }
    catch (const std::string& e) 
    {
      QDPIO::cerr << InlineHutchinsonTraceEstimatorEnv::name << ": forward prop: error message: " << e 
		  << std::endl;
      QDP_abort(1);
    }
    QDPIO::cout << "Forward propagator successfully parsed" << std::endl;

    try
    {
      // Snarf a copy
       P2 = TheNamedObjMap::Instance().getData<LatticePropagator>(params.named_obj.prop_2);
	
      // Snarf the frwd prop info. This is will throw if the frwd prop id is not there
      XMLReader PropXML, PropRecordXML;
      TheNamedObjMap::Instance().get(params.named_obj.prop_2).getFileXML(PropXML);
      TheNamedObjMap::Instance().get(params.named_obj.prop_2).getRecordXML(PropRecordXML);
      // Try to invert this record XML into a ChromaProp struct
      {
	read(PropRecordXML, "/Propagator/ForwardProp", prop_header_1);
	read(PropRecordXML, "/Propagator/PropSource", source_header_1);
      }
      // Sanity check - write out the norm2 of the forward prop in the j_decay direction
      // Use this for any possible verification
      {
	multi1d<Double> PropCheck = 
	  sumMulti( localNorm2( P2 ), phases_nomom2.getSet() );
	QDPIO::cout << "forward propagator check = " << PropCheck[0] << std::endl;
	// Write out the forward propagator header
	push(XmlOut, "ForwardProp");
	write(XmlOut, "PropXML", PropXML);
	write(XmlOut, "PropRecordXML", PropRecordXML);
	write(XmlOut, "PropCheck", PropCheck);
	pop(XmlOut);
      }
    }
    catch( std::bad_cast ) 
    {
      QDPIO::cerr << InlineHutchinsonTraceEstimatorEnv::name << ": caught dynamic cast error" 
		  << std::endl;
      QDP_abort(1);
    }
    catch (const std::string& e) 
    {
      QDPIO::cerr << InlineHutchinsonTraceEstimatorEnv::name << ": forward prop: error message: " << e 
		  << std::endl;
      QDP_abort(1);
    }
    QDPIO::cout << "Forward propagator successfully parsed" << std::endl;

    
    //#################################################################################//
    // Construct Building Blocks                                                       //
    //#################################################################################//
    QDP::StopWatch swatch;
    swatch.reset();
    
    XMLBufferWriter file_xml;
    push(file_xml, "HUTCHINSON_TRACE_ESTIMATOR");
    write(file_xml, "Param", params.param);
    write(file_xml, "Config", gauge_xml);
    pop(file_xml);
    //QDPFileWriter qio_file(file_xml, params.param.file_name,QDPIO_SINGLEFILE, 
	//		   QDPIO_SERIAL, QDPIO_OPEN); 
    XMLBufferWriter prop_xml;
    push(prop_xml,"QuarkPropagator");
    //write(prop_xml,"mom",mom);
    //write(prop_xml,"origin",t_src);
    //write(prop_xml,"t_dir",t_dir);
    pop(prop_xml);
    
    Seed ran_seed;
    QDP::RNG::savern(ran_seed);

    LatticeReal rnd1;
    random(rnd1); //Salva un random en cada punto del Lattice

    
    //restore the see
    QDP::RNG::setrn(ran_seed);

    LatticeFermion entrada;
    LatticeFermion salida;
   
    multi1d <LatticeComplex> temp;
    temp.resize(100);
    
       for(int i=0; i<100; i++){ // i in range(0,100)
          //salida=random(-1,1)*entrada;
          z2_src_t(entrada); 
          salida=entrada;
          temp[i] += innerProduct(entrada,salida);       
       }
       
      Complex acumulado=0;
      
      for(int i=0; i<100; i++){ // i in range(0,100):
        //QDPInternal::globalSum(temp[i]); //the globalSum function make the reduction and also the sum of the rest of the system
        acumulado+=sum(temp[i]);
      }
      acumulado/=100;
        
      int entradasDiagonal = QDP::Layout::vol()*12;
      Complex esperado=acumulado/entradasDiagonal; //Este deberia ser 1 real

      QDPIO::cout << "El valor esperado es " << esperado.elem().elem().elem().real()<<"\n";
      
// x = ( a==b ? c : d ); operador ternario



//PRODUCTO DE MATRICES RANDOM C++
//#include <iostream> para poder usar numeros aleatorios
//#include <sdtlib.h> para poder usar numeros aleatorios
//using namespace std; para poder usar funciones cout y cin
//int main (int argc, char** argv){
//	int filas = 3, columnas = 3; variables de tipo entero inicializadas en 3 para especificar el tamaño de las matrices	
//	int matrizA[filas][columnas];
//	int matrizB[filas][columnas];
//	int matrizR[filas][columnas];
//	int matrizT[columnas][filas]; cambiamos filas por columnas
//	int esperado;
//	srand(time(NULL)); funcion para poder utilizar el random en c++
//
//	Llenar matrices mediante ciclos
//	for(int i=0; i<filas; i++){            Recorremos filas
//		for(int j=0; j<columnas; j++){ Recorremos columnas
//			matrizA[i][j] = rand()%2; la funcion rand()%2 lo que hace es guardar números aleatorios entre 0 y 1
//			//matrizB[i][j] = rand()%2;		
//		}
//	}
//	Matriz transpuesta
//	for(int i=0; i<filas; i++){
//		for(int j=0; j<columnas; j++){
//			matrizT[j][i] = matrizA[i][j];
//		}
//	}
//	Producto de matriz por su transpuesta
//	for(int i=0; i<filas; i++){
//		for(int j=0; j<columnas; j++){
//			matrizR[i][j] = 0;
//			for(int k=0; k<filas; k++){
//				matrizR[i][j] += matrizT[i][k] * matrizA[k][j];
//			}
//		}
//	
//	}
//	return 0;
//}


// * LatticeHalfFermion entrada;
// * LatticeHalfFermion salida=0;
// *   multi1d <LatticeComplex> temp;
// *   temp.resize(100);
// *
// *   for i in range(0,100)
// *      //salida=random(-1,1)*entrada;    
// *
// *      for s1 in range(0,4)
// *            for c1 in range(0,3)
// *                salida.elem(s1).elem(c1).elem().real()=random(-1,1);
// *                //salida.elem(s1).elem(c1).elem().imag()=random(-1,1);
// *
// *      entrada = salida;
// *      //salida= Dirac*salida;// 
// *      //temp[i] +=  entrada*entrada;
// *      temp[i] +=  entrada*salida;
// *      
// *  complex acumulado=0;   
// *  for i in range(0,100):     
// *    acumulado+=temp[i];
// *
// *  acumulado/=100;
// *
// *  int entradasDiagonal = QDP::Layout::vol()*12;
// *  
// *  esperado=acumulado/entradasDiagonal; //Este deberia ser 1 real




   
     //end ale
    //close(qio_file);
    //QDPIO::cout << "finished calculating HUTCHINSON_ESTIMATOR_TRACE"
	//	<< "  time= "
	//	<< swatch.getTimeInSeconds() 
	//	<< " secs" << std::endl;
    pop(XmlOut);   // HUTCHINSON_ESTIMATOR_TRACE
    //snoop.stop();
    //QDPIO::cout << InlineHutchinsonEstimatorTraceEnv::name << ": total time = "
	//	<< snoop.getTimeInSeconds() 
	//	<< " secs" << std::endl;

    QDPIO::cout << InlineHutchinsonTraceEstimatorEnv::name << ": ran successfully" << std::endl;
    END_CODE();
  } 
}


//SIMPLE ESTIMATION OF THE TRACE OF SQUARE MATRIX I
//We can estimate the trace of Square Matrix A with num?queries many matrix/vector products.
//Implements Hutchinson's estimator. Random sign vectors are used.
//Generate a random sign matrix
//function trace_est = simple_hutchinson(A, num_queries)
//	S = 2*randi(2, size(A,1), num_queries)-3;
//	
// 	trace_est = trace(S*(A*S))/ num_queries;
//end



