//
// Created by captain on 4/28/2022.
//

#ifndef INTEGRATE_PHYSX_PHYSIC_MATERIAL_H
#define INTEGRATE_PHYSX_PHYSIC_MATERIAL_H


class PhysicMaterial {
public:
    PhysicMaterial(float static_friction, float dynamic_friction, float restitution);

    float static_friction() const{return static_friction_;}
    void set_static_friction(float static_friction){static_friction_ = static_friction;}

    float dynamic_friction() const{return dynamic_friction_;}
    void set_dynamic_friction(float dynamic_friction){dynamic_friction_ = dynamic_friction;}

    float restitution() const{return restitution_;}
    void set_restitution(float restitution){restitution_ = restitution;}

private:
    float static_friction_;
    float dynamic_friction_;
    float restitution_;
};


#endif //INTEGRATE_PHYSX_PHYSIC_MATERIAL_H
