/*! \file
 *  \brief Hyp fermstate params
 */

#include "actions/ferm/fermstates/hyp_fermstate_params.h"


namespace Chroma 
{

  HypFermStateParams::HypFermStateParams() 
  {
    n_smear = 0;
    smear_in_this_dirP.resize(Nd);
    for(int i=0; i<Nd; i++) smear_in_this_dirP[i] = true;
    alpha1;
    alpha2;
    alpha3;
    BlkMax;
    BlkAccu;
  }


  HypFermStateParams::HypFermStateParams(XMLReader& in, const std::string& path) 
  {
    try
    {
      XMLReader paramtop(in, path);

      int version = 1;
      if( paramtop.count("version") == 1 )
	read(paramtop, "version", version);

      read(paramtop, "n_smear", n_smear);
      read(paramtop, "alpha1", alpha1);
      read(paramtop, "alpha2", alpha2);
      read(paramtop, "alpha3", alpha3);
      read(paramtop, "BlkMax", BlkMax);
      read(paramtop, "BlkAccu", BlkAccu);
      int orthog_dir = -1;
      read(paramtop, "orthog_dir", orthog_dir);
      for(int i=0; i<Nd; i++) 
        if(i == orthog_dir) smear_in_this_dirP[i] = false;
    }
    
    catch(const std::string& e) 
    { 
      QDPIO::cout << "Failed to read hyp action XML:" << e << std::endl;
    }

  }

  void read(XMLReader& xml, const std::string& path, HypFermStateParams& p)
  {
    HypFermStateParams tmp_p(xml, path);
    p = tmp_p;
  }

  void write(XMLWriter& xml, const std::string& path, const HypFermStateParams& p) 
  {
    push(xml, path);
    int version = 1;
    write(xml, "version", version);
    write(xml, "n_smear", p.n_smear);
    write(xml, "alpha1", p.alpha1);
    write(xml, "alpha2", p.alpha2);
    write(xml, "alpha3", p.alpha3);
    write(xml, "BlkMax", p.BlkMax);
    write(xml, "BlkAccu", p.BlkAccu);
    write(xml, "smear_in_this_dirP", p.smear_in_this_dirP);

    pop(xml);
  }


}    
