#include "Track.hh"


Track::Track(Geom* geom):m_geom(geom){}


void Track::SetIncidentParticle(string particleName)
{
    auto res1=find(fVParticleName.begin(),fVParticleName.end(),particleName);

    if(res1 != fVParticleName.end())
    {
        fParticleName=particleName;
        cout<<"Particle=====>"<<fParticleName<<endl;
    }
    else{
        cout<<"=======> Error"<<endl;
    }
}

void Track::SetMomentum(double momentum)
{
    m_momentum = momentum*1e+9;
}


void Track::SetBetaGamma(double bg)
{
    m_bg = bg;
}


void Track::SetTheta(double theta)
{
    m_theta = theta;
    //for(auto &i : fDirection)
    fDirection[0]=sin(m_theta);
    fDirection[1]=cos(m_theta);
    fDirection[2]=0;
}
void Track::SetDirection(double dx,double dz,double dy)
{
    fDirection[0]=dx;
    fDirection[1]=dz;
    fDirection[2]=dy;
}
