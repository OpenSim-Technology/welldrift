#ifndef H_WellSimulator_ABSTRACTWELL
#define H_WellSimulator_ABSTRACTWELL

#include <WellVector.h>
#include <NodeCoordinates.h>
#include <Typedefs.h>
#include <BoostWrapper/SmartPointer.h>

// Namespace =======================================================================================
namespace WellSimulator {

// AbstractWell ===================================================================================
class AbstractWell
{
//--------------------------------------------------------------------------------- Type Definitions
public:
	//typedef double						real_type;
	//typedef unsigned int				uint_type;
	////typedef WellSimulator::WellVector	vector_type;
 //   typedef std::vector<double>				vector_type;
	//typedef NodeCoordinates				coord_type;

//------------------------------------------------------------------------- Constructor & Destructor
public:
	AbstractWell(){}
	virtual ~AbstractWell(){}
//---------------------------------------------------------- M�todos de interface com o reservat�rio
public:
	virtual void set_size(const uint_type& p_nnodes) = 0;
	virtual void initialize_flow(
								 SharedPointer<vector_type> p_oil_flow_vector,
								 SharedPointer<vector_type> p_water_flow_vector,
								 SharedPointer<vector_type> p_gas_flow_vector
								 ) = 0;
	virtual void set_radius(const real_type& p_radius) = 0;
	virtual void read_coordinates( std::ifstream& p_infile) = 0;
	virtual coord_type* coordinates(uint_type p_index) = 0;
	virtual real_type* pressure(uint_type p_index) = 0;
	virtual real_type radius() const = 0;
	virtual uint_type number_of_nodes() const = 0;
	virtual void solve() = 0;

};

// Namespace =======================================================================================
} // namespace WellSimulator

#endif // H_WELLRES10_ABSTRACTWELL

/*
{
.
.    Cria��o dos po�os, armazenamento em wells (vetor de po�os)
.
	for( int i = 0; i < number_f_wells; ++i ){
		calculaFluxoNoPo�o( i, flow_vector );
		wells[i].initialize_flow( flow_vector );
	}
	while( residuo maior que tolerancia ){
		zeraSistemaDeEqua��esDoReservatorio();
		for( int i = 0; i < number_f_wells; ++i ){
			wells[i].solve();
			double radius = wells[i].radius();
			NodeCoordinates* coord;
			double* wellPressure;
			for( int j = 0; j < wells[i].number_of_nodes(); ++j ){
				coord = well[i].coordinates( j );
				wellPressure = well[i].pressure( j );

				Localiza o elemento que cont�m este n� do po�o;
				Calcula vaz�o utilizando a press�o do reservat�rio e wellPressure;
				Adiciona a vazao no sistema de equa��es do reservat�rio;
			}
		}
		terminaAContrucaoDoSistemaDeEquacoesDoRes();
		resolveOReservatorio();
	}
.
.
.}

*/
