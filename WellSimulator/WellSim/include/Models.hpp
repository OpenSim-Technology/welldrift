#ifndef HPP_WellSimulator_MODELS
#define HPP_WellSimulator_MODELS

#include <boost/math/special_functions/fpclassify.hpp>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <ReservoirConstants.hpp>
// =================================================================================================

namespace WellSimulator {
#define IsNan boost::math::isnan
typedef double float64;

// =================================================================================================
class ConstantModel
{
public:
    ConstantModel(const float64& p_constant_value)
        : m_constant_value(p_constant_value)
    {}

    float64 get_constant_value() const{
        return this->m_constant_value;
    }

    virtual ~ConstantModel(){}

protected:
    float64 m_constant_value;
}; //class ConstantModel;

class PowerModel{
public:
    PowerModel(const float64& p_alpha, const float64& p_power)
        : m_alpha(p_alpha)
        , m_power(p_power)
        , m_ref_value(0.0)
        , m_computed_value(0.0)
    {}

    void set_alpha(const float64& p_alpha){
        this->m_alpha = p_alpha;
        this->compute_value();
    }

    void set_power(const float64& p_power){
        this->m_power = p_power;
        this->compute_value();
    }

    void set_ref_value(const float64& p_ref_value){
        this->m_ref_value = p_ref_value;
        this->compute_value();
    }

    float64 get_alpha() const{
        return this->m_alpha;
    }

    float64 get_power() const{
        return this->m_power;
    }

    float64 get_ref_value() const{
        return this->m_ref_value;
    }

    float64 get_computed_value(){
        return this->m_computed_value;
    }

    virtual ~PowerModel(){}

protected:
    float64 m_alpha;
    float64 m_power;
    float64 m_ref_value;
    float64 m_computed_value;

private:

    void compute_value(){
        this->m_computed_value = this->m_alpha*pow(this->m_ref_value, this->m_power);
    }
}; //class PowerModel;


class IDensityModel
{
public:
    inline float64 compute_density(float64 p_pressure) {
        return this->do_compute_density(p_pressure);
    }

private:
    virtual float64 do_compute_density(float64 p_pressure) = 0;
};

//--------------------------------------------------------------------------------------------------

class ConstantDensityModel
    : public IDensityModel
{
public:
    ConstantDensityModel(float64 p_density)
        : m_density(p_density)
    {
    }

public:
    inline float64 compute_density(float64 p_pressure) {
        return this->m_density;
    }

private:
    virtual float64 do_compute_density(float64 p_pressure) {
        return this->compute_density(p_pressure);
    }

protected:
    float64 m_density;
};

class WellCompressibleDensityModel
    : public IDensityModel
{
public:
    WellCompressibleDensityModel(float64 p_standard_density, float64 p_standard_pressure, float64 p_standard_sound_speed)
        : m_standard_density	(p_standard_density),
		  m_standard_pressure	(p_standard_pressure),
		  m_standard_sound_speed(p_standard_sound_speed)
		  
    {
    }

public:
    inline float64 compute_density(float64 p_pressure) {
        return m_standard_density + (p_pressure - m_standard_pressure)/(m_standard_sound_speed*m_standard_sound_speed);
		// Density is calculated as:
		// rho = ref_rho + (Pressure - ref_Pressure)/a^2
		// where 'a' is the fluid sound speed,
		// ref_rho and ref_Pressure are the reference
		// density and Pressure, respectively, when the 
		// fluid is liquid. In case of gases, ref_rho = 0, 
		// ref_Pressure = 0 and a^2 = R*T_ref 
    }

private:
    virtual float64 do_compute_density(float64 p_pressure) {
        return this->compute_density(p_pressure);
    }

protected:
    float64 m_standard_density;
	float64 m_standard_pressure;
	float64 m_standard_sound_speed;
};

class CompressibleDensityModel
    : public IDensityModel
{
public:
    CompressibleDensityModel(float64 p_standard_density)
        : m_standard_density(p_standard_density)
    {
    }

public:
    inline float64 compute_density(float64 p_form_vol_factor) {
        return this->m_standard_density/p_form_vol_factor;
    }

private:
    virtual float64 do_compute_density(float64 p_form_vol_factor) {
        return this->compute_density(p_form_vol_factor);
    }

protected:
    float64 m_standard_density;
};

class OilDensityModel
    : public IDensityModel
{
public:
    OilDensityModel(float64 p_oil_standard_density, float64 p_gas_standard_density)
        : m_oil_standard_density(p_oil_standard_density),
          m_gas_over_oil_standard_density(p_gas_standard_density/p_oil_standard_density),
          m_solubility(0.0)
    {
    }

public:
    void set_solubility(float64 p_solubility) {
        this->m_solubility = p_solubility;
    }

    inline float64 compute_density(float64 p_form_vol_factor) {
        return (1 + this->m_gas_over_oil_standard_density*this->m_solubility)*
               (this->m_oil_standard_density/p_form_vol_factor);
    }

private:
    virtual float64 do_compute_density(float64 p_form_vol_factor) {
        return this->compute_density(p_form_vol_factor);
    }

protected:
    float64 m_oil_standard_density;
    float64 m_gas_over_oil_standard_density;
    float64 m_solubility;
};


//--------------------------------------------------------------------------------------------------

class IViscosityModel
{
public:
    inline float64 compute_viscosity(float64 p_pressure) {
        return this->do_compute_viscosity(p_pressure);
    }

private:
    virtual float64 do_compute_viscosity(float64 p_pressure) = 0;
};

class PowerViscosityModel
    : public IViscosityModel
{
public:
    PowerViscosityModel(float64 p_alpha, float64 p_exponent)
        : m_alpha(p_alpha),
          m_exponent(p_exponent)
    {
    }

public:
    inline float64 compute_viscosity(float64 p_pressure) {
        return this->m_alpha*pow(p_pressure, this->m_exponent);
    }

private:
    virtual float64 do_compute_viscosity(float64 p_pressure) {
        return this->compute_viscosity(p_pressure);
    }

protected:
    float64 m_alpha;
    float64 m_exponent;
};

// =================================================================================================


class IRelativePermeabilityModel
{
public:
    inline float64 compute_relative_permeability(float64 p_phase_saturation) {
        return this->do_compute_relative_permeability(p_phase_saturation);
    }

private:
    virtual float64 do_compute_relative_permeability(float64 p_phase_saturation) = 0;
};


class PowerRelativePermeabilityModel
    : public IRelativePermeabilityModel
{
public:
    PowerRelativePermeabilityModel(
        float64 p_minimum_saturation, //!< when water, it is the irreducible saturation; when oil, it is the residual saturation
        float64 p_maximum_saturation,
        float64 p_maximum_relative_permeability, //!< at the maximum admissible saturation
        float64 p_exponent = 1.0
        )
        : m_minimum_saturation(p_minimum_saturation)
        , m_maximum_saturation(p_maximum_saturation)
        , m_maximum_relative_permeability(p_maximum_relative_permeability)
        , m_exponent(p_exponent)
    {
        this->m_alpha = 1.0 / (m_maximum_saturation - m_minimum_saturation);
    }

    float64 compute_relative_permeability(float64 p_phase_saturation)
    {
        if (p_phase_saturation < (this->m_minimum_saturation)) {
            return 0.0;
        }
        if (p_phase_saturation > (this->m_maximum_saturation)) {
            return this->m_maximum_relative_permeability;
        }

        float64 t = (p_phase_saturation - m_minimum_saturation) * this->m_alpha;
        float64 kr = std::pow(t, this->m_exponent) * m_maximum_relative_permeability;

        return kr;
    }

private:
    virtual float64 do_compute_relative_permeability(float64 p_phase_saturation) {
        return this->compute_relative_permeability(p_phase_saturation);
    }

protected:
    float64 m_minimum_saturation;
    float64 m_maximum_saturation;
    float64 m_maximum_relative_permeability; //!< at the maximum admissible water saturation
    float64 m_exponent;

    float64 m_alpha; //!< cached value to optimize some calculations
};


// =================================================================================================

class ISolubilityModel
{
public:
    inline float64 compute_solubility(float64 p_pressure, float64 p_oil_mass_fraction, float64 p_water_mass_fraction) {
        return this->do_compute_solubility(p_pressure, p_oil_mass_fraction, p_water_mass_fraction);
    }

private:
    virtual float64 do_compute_solubility(float64 p_pressure, float64 p_oil_mass_fraction, float64 p_water_mass_fraction) = 0;
};

class PowerSolubilityModel : public ISolubilityModel{
public:
    PowerSolubilityModel(
                         float64 p_power, float64 p_alpha, 
                         float64 p_oil_standard_density, 
                         float64 p_gas_standard_density
                         ) : m_power(p_power), m_alpha(p_alpha),
                             m_oil_over_gas_standard_density(p_oil_standard_density/p_gas_standard_density)
    {}

    inline float64 compute_solubility(float64 p_pressure, float64 p_oil_mass_fraction, float64 p_gas_mass_fraction){
        double max_solubility = this->m_oil_over_gas_standard_density*p_gas_mass_fraction/
            p_oil_mass_fraction;
        double model_solubility = this->m_alpha*pow(p_pressure, this->m_power);

        return max_solubility >= model_solubility ? model_solubility : max_solubility;
    }

private:
    virtual float64 do_compute_solubility(float64 p_pressure, float64 p_oil_mass_fraction, float64 p_water_mass_fraction){
        return this->compute_solubility(p_pressure, p_oil_mass_fraction, p_water_mass_fraction);
    }

protected:
    float64 m_power;
    float64 m_alpha;
    float64 m_oil_over_gas_standard_density;
};


// =================================================================================================

class IFormationVolumeFactorModel
{
public:
    inline float64 compute_formation_volume_factor(float64 p_pressure) {
        return this->do_compute_formation_volume_factor(p_pressure);
    }

private:
    virtual float64 do_compute_formation_volume_factor(float64 p_pressure) = 0;
};

class LiquidFormationVolumeFactorModel : public IFormationVolumeFactorModel
{
public:
    LiquidFormationVolumeFactorModel(
        float64 p_compressibility,
        float64 p_ref_pressure,
        float64 p_ref_formation_volume_factor
    ) : m_compressibility(p_compressibility),
        m_ref_pressure(p_ref_pressure),
        m_ref_formation_volume_factor(p_ref_formation_volume_factor)
    {}

    inline float64 compute_formation_volume_factor(float64 p_pressure) {
        return this->do_compute_formation_volume_factor(p_pressure);
    }

private:
    virtual float64 do_compute_formation_volume_factor(float64 p_pressure){
        return this->m_ref_formation_volume_factor/(1 + this->m_compressibility*(p_pressure - this->m_ref_pressure));
        //             B_ref
        //  B(P)= ----------------
        //        1 + c(P - P_ref)
    }

protected:
    float64 m_compressibility;
    float64 m_ref_pressure;
    float64 m_ref_formation_volume_factor;
};

class GasFormationVolumeFactorModel : public IFormationVolumeFactorModel
{
public:
    GasFormationVolumeFactorModel(
        float64 p_ref_pressure,
        float64 p_ref_formation_volume_factor
    ) : m_ref_pressure(p_ref_pressure),
        m_ref_formation_volume_factor(p_ref_formation_volume_factor)
    {}


    inline float64 compute_formation_volume_factor(float64 p_pressure) {
        return this->do_compute_formation_volume_factor(p_pressure);
    }

private:
    virtual float64 do_compute_formation_volume_factor(float64 p_pressure){
        return (this->m_ref_formation_volume_factor*p_pressure)/(2*p_pressure - this->m_ref_pressure);
        //            B_ref                   assuming that the isothermic compressibility for a
        //   B(P)= -----------                gas equals the inverse of the pressure(pg 24, "Engenharia
        //          2 - P_ref                 de Reservatórios de Petróleo") )...thus, manipulating
        //              -----                 the expression used to calculate the LiquidFormation... 
        //                P                   we came withthis expression.
    }

protected:
    float64 m_ref_pressure;
    float64 m_ref_formation_volume_factor;
};




class IInterfacialTensionModel{
public:

    virtual float64 compute_interfacial_tension(const float64& p_pressure) = 0;

    virtual ~IInterfacialTensionModel(){}
}; //class IInterfacialTensionModel

class ConstantInterfacialTensionModel:
    public IInterfacialTensionModel, ConstantModel
{
public:
    ConstantInterfacialTensionModel(const float64& p_value)
        : ConstantModel(p_value)
    {}
    virtual float64 compute_interfacial_tension(const float64& p_pressure){
        return get_constant_value();
    }
    virtual ~ConstantInterfacialTensionModel(){}
}; //class ConstantInterfacialTensionModel

class BeggsGasOilInterfacialTensionModel:
    public IInterfacialTensionModel
{
public:
    BeggsGasOilInterfacialTensionModel(const float64& p_temperature/*[K]*/, const float64& p_relative_density_std)
        : IInterfacialTensionModel(), m_temperature(9.0*p_temperature/5.0 - 459.67)/*[F]*/, m_API_degree(141.5/p_relative_density_std - 131.5)
    {}

    virtual float64 compute_interfacial_tension(const float64& p_pressure){
        float64 sigma_68F  = (39.0 - 0.2571*m_API_degree);
        float64 sigma_100F = (37.5 - 0.2571*m_API_degree);
        float64 sigma = 0.0;
        float64 factor = WellConstants::convert_Pa_to_psi();
        float64 C = 1.0 - 0.024*pow(factor*p_pressure,0.45);
        factor = WellConstants::convert_Dynes_per_cm_to_Pa_m();
        if (m_temperature > 100.0){
            m_temperature = 100.0;
        }
        else if (m_temperature < 68.0){
            m_temperature = 68.0;
        }
        sigma = factor*C*(sigma_68F - (m_temperature - 68.0)*(sigma_68F - sigma_100F)/32.0);
        // Note: the minimal interfacial tension is "factor", however this condition is already inputed on wellsimulator
        return sigma /*< 1.0*factor ? factor : sigma*/;
    }
    virtual ~BeggsGasOilInterfacialTensionModel(){}
protected:
    float64 m_temperature;
    float64 m_API_degree;

}; //class BeggsGasOilInterfacialTensionModel

class BeggsGasWaterInterfacialTensionModel:
    public IInterfacialTensionModel
{
public:
    BeggsGasWaterInterfacialTensionModel(const float64& p_temperature/*[K]*/)
        : IInterfacialTensionModel(), m_temperature(9.0*p_temperature/5.0 - 459.67)/*[�F]*/
    {}

    virtual float64 compute_interfacial_tension(const float64& p_pressure){
        float64 factor = WellConstants::convert_Pa_to_psi();
        float64 sigma_74F  = 75.0 - 1.108 *pow(factor*p_pressure,0.349);
        float64 sigma_280F = 53.0 - 0.1048*pow(factor*p_pressure,0.637);
        float64 sigma = 0.0;

        factor = WellConstants::convert_Dynes_per_cm_to_Pa_m();
        if (m_temperature > 280.0){
            m_temperature = 280.0;
        }
        else if (m_temperature < 74.0){
            m_temperature = 74.0;
        }
        sigma = factor*(sigma_74F - (m_temperature - 74.0)*(sigma_74F - sigma_280F)/206.0);
        // Note: the minimal interfacial tension is "factor", however this condition is already inputed on wellsimulator
        return sigma /*< 1.0*factor ? factor : sigma*/;
    }
    virtual ~BeggsGasWaterInterfacialTensionModel(){}
protected:
    float64 m_temperature;

}; //class BeggsGasWaterInterfacialTensionModel

//----Drift Flux Models----------------------------------------------------------------------------//

//----Drift Velocity Models------------------------------------------------------------------------//
class IDriftVelocityModel{
public:
    virtual float64 compute_drift_velocity() = 0;
    virtual void set_volume_fraction(const float64& p_volume_fraction){}
    virtual void set_profile_parameter(const float64& p_profile_parameter){}
    virtual void set_characteristic_velocity(const float64& p_characteristic_velocity){}
    virtual void set_dispersed_density(const float64& p_dispersed_density){}
    virtual void set_not_dispersed_density(const float64& p_not_dispersed_density){}
    virtual void set_Ku_critical(const float64& p_Ku_critical){}

    virtual ~IDriftVelocityModel(){}
}; //class IDriftVelocityModel

class ConstantDriftVelocityModel:
    public IDriftVelocityModel, public ConstantModel
{
public:
    ConstantDriftVelocityModel(const float64& p_ref_drift_velocity)
        : IDriftVelocityModel()
        , ConstantModel(p_ref_drift_velocity)
    {}

    virtual float64 compute_drift_velocity(){
        return get_constant_value();
    }

    virtual ~ConstantDriftVelocityModel(){}
}; //class ConstantDriftVelocityModel

class GasVolumeFractionDriftVelocityModel:
    public IDriftVelocityModel, public PowerModel
{
public:
    GasVolumeFractionDriftVelocityModel(const float64 p_alpha, const float64 p_power)
        : IDriftVelocityModel()
        , PowerModel(p_alpha, p_power)
    {}

    virtual void set_gas_volume_fraction(const float64& p_gas_volume_fraction){
        set_ref_value(1 - p_gas_volume_fraction);
    }

    virtual float64 compute_drift_velocity(){
        return get_computed_value();
    }

    virtual ~GasVolumeFractionDriftVelocityModel(){}
}; //class GasVolumeFractionDriftVelocityModel

class ShiGasLiquidDriftVelocityModel:
    public IDriftVelocityModel
{
public:
    ShiGasLiquidDriftVelocityModel(const float64& p_a1, const float64& p_a2)
        : IDriftVelocityModel(), m_a1(p_a1), m_a2(p_a2)
    {}

    virtual float64 compute_drift_velocity(){
        float64 k = 0.0;
        float64 k_upp = m_Ku_critical;
        float64 k_low = 1.53/m_profile_parameter;
        float64 o_Vd = 0.0;
        if(m_vol_frac <= m_a1){
            k = k_low;
        }else if(m_vol_frac >= m_a2){
            k = k_upp;
        }else{
            k = k_upp - ((m_a2 - m_vol_frac)/(m_a2 - m_a1))*(k_upp-k_low);
        }


        //if (m_vol_frac*m_profile_parameter > 1.0 || m_vol_frac*m_profile_parameter < 0.0){
        //    return 0.0;
        //}

        //m_vol_frac = (m_vol_frac > 1.0) ? 1.0 : m_vol_frac;
        //m_vol_frac = (m_vol_frac < 0.0) ? 0.0 : m_vol_frac;

        o_Vd = (1.0 - m_vol_frac*m_profile_parameter)*m_profile_parameter*k*m_characteristic_velocity/
            (m_vol_frac*m_profile_parameter*sqrt(m_dispersed_density/m_not_dispersed_density) + 1.0 - m_vol_frac*m_profile_parameter);
        if (m_dispersed_density/m_not_dispersed_density < 0.0 || m_dispersed_density/m_not_dispersed_density > 1.0 /*|| m_vol_frac >= 1.0*/){
            std::cout << std::setprecision(18) << std::fixed << std::scientific <<
                "\n m_vol_frac = " << m_vol_frac <<
                "\n o_Vd = " << o_Vd <<
                "\n k = " << k <<
                "\n m_characteristic_velocity = " << m_characteristic_velocity <<
                "\n m_profile_parameter = " << m_profile_parameter <<
                "\n m_dispersed_density = " << m_dispersed_density <<
                "\n m_not_dispersed_density = " << m_not_dispersed_density <<
                "\n m_dispersed_density/m_not_dispersed_density = " << m_dispersed_density/m_not_dispersed_density;
        }

        //if(o_Vd < 0.0){
        //    return 0.0;
        //}
        
        if(IsNan(o_Vd) /* || o_Vd < 0.0*/)
        {
            //log_system::enable_log(1)
            std::cout << std::setprecision(17) << std::fixed << std::scientific <<
                "\nm_vol_frac = " << m_vol_frac <<
                "\nm_profile_parameter = " << m_profile_parameter <<
                "\n(1.0 - m_vol_frac*m_profile_parameter) = " << (1.0 - m_vol_frac*m_profile_parameter) <<
                "\n(m_vol_frac*m_profile_parameter*sqrt(m_dispersed_density/m_not_dispersed_density) + 1.0 - m_vol_frac*m_profile_parameter) = " <<
                (m_vol_frac*m_profile_parameter*sqrt(fabs(m_dispersed_density/m_not_dispersed_density)) + 1.0 - m_vol_frac*m_profile_parameter) <<
                "\n o_Vd = " << o_Vd ;
            //log_system::disable_log(1)
            system("PAUSE");
        }

        return  o_Vd;
    }

    virtual void set_volume_fraction(const float64& p_volume_fraction){
        this->m_vol_frac = p_volume_fraction;
    }

    virtual void set_profile_parameter(const float64& p_profile_parameter){
        this->m_profile_parameter = p_profile_parameter;
    }

    virtual void set_characteristic_velocity(const float64& p_characteristic_velocity){
        this->m_characteristic_velocity = p_characteristic_velocity;
    }

    virtual void set_dispersed_density(const float64& p_dispersed_density){
        this->m_dispersed_density = p_dispersed_density;
    }

    virtual void set_not_dispersed_density(const float64& p_not_dispersed_density){
        this->m_not_dispersed_density = p_not_dispersed_density;
    }

    virtual void set_Ku_critical(const float64& p_Ku_critical){
        this->m_Ku_critical = p_Ku_critical;
    }

    virtual ~ShiGasLiquidDriftVelocityModel(){}
protected:
    float64 m_a1;
    float64 m_a2;
    float64 m_vol_frac;
    float64 m_profile_parameter;
    float64 m_characteristic_velocity;
    float64 m_dispersed_density;
    float64 m_not_dispersed_density;
    float64 m_Ku_critical;
}; //class ShiGasLiquidDriftVelocityModel

class ShiOilWaterDriftVelocityModel:
    public IDriftVelocityModel
{
public:
    ShiOilWaterDriftVelocityModel()
        : IDriftVelocityModel()
    {}

    virtual float64 compute_drift_velocity(){
        return 1.53*m_characteristic_velocity*pow( 1.0 - m_vol_frac , 2.0 );
    }

    virtual void set_volume_fraction(const float64& p_volume_fraction){
        this->m_vol_frac = p_volume_fraction;
    }

    virtual void set_characteristic_velocity(const float64& p_characteristic_velocity){
        this->m_characteristic_velocity = p_characteristic_velocity;
    }

    virtual ~ShiOilWaterDriftVelocityModel(){}
protected:
    float64 m_vol_frac;
    float64 m_characteristic_velocity;
}; //class ShiOilWaterDriftVelocityModel

//----Profile Parameter Models---------------------------------------------------------------------//
class IProfileParameterModel{
public:
    virtual float64 compute_profile_parameter() = 0;
    virtual void set_volume_fraction(const float64& p_volume_fraction){}
    virtual void set_mixture_velocity(const float64& p_mixture_velocity){}
    virtual void set_flooding_velocity(const float64& p_flooding_velocity){}

    virtual ~IProfileParameterModel(){}
};//class IProfileParameterModel

class ConstantProfileParameterModel:
    public IProfileParameterModel, public ConstantModel
{
public:
    ConstantProfileParameterModel(const float64& p_ref_profile_parameter)
        : IProfileParameterModel()
        , ConstantModel(p_ref_profile_parameter)
    {}

    virtual float64 compute_profile_parameter(){
        return get_constant_value();
    }

    virtual ~ConstantProfileParameterModel(){}
};//class ConstantProfileParameter

class ShiOilWaterProfileParameterModel: public IProfileParameterModel{
public:
    ShiOilWaterProfileParameterModel(const float64& p_A, const float64& p_B1, const float64& p_B2)
        : IProfileParameterModel(), m_A(p_A), m_B1(p_B1), m_B2(p_B2)
    {}

    virtual float64 compute_profile_parameter(){
        if(m_vol_frac <= m_B1){
            return m_A;
        } else if(m_vol_frac >= m_B2){
            return 1.0;
        } else {
            return m_A - (m_A - 1.0)*(m_vol_frac - m_B1)/(m_B2 - m_B1);
        }
    }

    virtual void set_volume_fraction(const float64& p_volume_fraction)
    {
        this->m_vol_frac = p_volume_fraction;
    }


    virtual ~ShiOilWaterProfileParameterModel(){}
protected:
    float64 m_A;
    float64 m_B1;
    float64 m_B2;
    float64 m_vol_frac;
};//class ShiOilWaterProfileParameterModel
#undef max 
#undef min
class ShiGasLiquidProfileParameterModel: public IProfileParameterModel{
public:
    ShiGasLiquidProfileParameterModel(const float64& p_A, const float64& p_B, const float64& p_Fv)
        : IProfileParameterModel(), m_A(p_A), m_B(p_B), m_Fv(p_Fv), m_vol_frac(0.0), m_mixture_velocity(0.0), m_flooding_velocity(0.0)
    {}

    virtual float64 compute_profile_parameter(){               
        float64 beta = std::max( m_vol_frac, m_Fv*m_vol_frac*std::abs(m_mixture_velocity)/m_flooding_velocity );
        
        float64 gamma = (beta - m_B)/(1.0 - m_B);
        if(gamma < 0.0) gamma = 0.0;
        if(gamma > 1.0) gamma = 1.0;

        return m_A/(1.0 + (m_A - 1.0)*pow(gamma,2.0));
    }

    virtual void set_volume_fraction(const float64& p_volume_fraction)
    {
        this->m_vol_frac = p_volume_fraction;
    }

    virtual void set_mixture_velocity(const float64& p_mixture_velocity)
    {
        this->m_mixture_velocity = p_mixture_velocity;
    }
    virtual void set_flooding_velocity(const float64& p_flooding_velocity)
    {
        this->m_flooding_velocity = p_flooding_velocity;
    }
    virtual ~ShiGasLiquidProfileParameterModel(){}
protected:
    float64 m_A;
    float64 m_B;
    float64 m_Fv;
    float64 m_vol_frac;
    float64 m_mixture_velocity;
    float64 m_flooding_velocity;
};//class ShiGasLiquidProfileParameterModel

} // namespace WellSimulator
#endif // HPP_WellSimulator_MODELS
