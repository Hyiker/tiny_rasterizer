#include "rasterizer/Shader.hpp"

using namespace Rasterizer;
RGBColor Rasterizer::normalFragmentShader(FragmentShaderPayload& payload) {
    RGBColor color = (payload.normal + Vec3(1.0f)) / 2.f;
    return color;
}
RGBColor Rasterizer::textureFragmentShader(FragmentShaderPayload& payload) {
    RGBColor texture_color;
    if (!payload.texture) {
        texture_color = RGBColor(1.0);
    } else {
        texture_color = payload.texture->getRGBBilinear(
            payload.texture_coord.x, payload.texture_coord.y);
    }
    // coefficients for ambient, diffuse and specular lighting
    Vec3 ka = payload.material->Ka;
    Vec3 kd = payload.texture ? texture_color : payload.material->Kd;
    Vec3 ks = payload.material->Ks;
    float shiness = payload.material->Ns;

    Vec3 eye_vec = (payload.eye_pos - payload.view_position).normalized();

    Vec3 ambient_light_intensity(0.01);
    RGBColor color;
    for (auto& light : payload.lights) {
        float light_point_dist_sq =
            (light.position - payload.view_position).norm2();
        Vec3 light_indensity = light.intensity / light_point_dist_sq;
        Vec3 light_vec = (light.position - payload.view_position).normalized();
        Vec3 half_vec = (light_vec + eye_vec).normalized();

        RGBColor ambient, diffuse, specular;
        ambient = ambient_light_intensity.cwiseProduct(ka);
        diffuse =
            kd.cwiseProduct(light_indensity) *
            std::max(0.0f, float(payload.normal.dot(light_vec.normalized())));
        // bling-phong specular shading model
        // https://www.cnblogs.com/bluebean/p/5299358.html
        specular =
            ks.cwiseProduct(light_indensity) *
            // negative fwd incident ray will be ignore
            std::pow(std::max(payload.normal.dot(half_vec), 0.0f), shiness);
        color = color + ambient + diffuse + specular;
    }

    return color.min(texture_color);
}

RGBColor Rasterizer::blingphongFragmentShader(FragmentShaderPayload& payload) {
    RGBColor texture_color = RGBColor(1.0f);
    // coefficients for ambient, diffuse and specular lighting
    Vec3 ka = Vec3(0.005);
    Vec3 kd = texture_color;
    Vec3 ks = Vec3(0.7937);
    float shiness = 150.0f;

    Vec3 eye_vec = (payload.eye_pos - payload.view_position).normalized();

    Vec3 ambient_light_intensity(10);
    RGBColor color;
    for (auto& light : payload.lights) {
        float light_point_dist_sq =
            (light.position - payload.view_position).norm2();
        Vec3 light_indensity = light.intensity / light_point_dist_sq;
        Vec3 light_vec = (light.position - payload.view_position).normalized();
        Vec3 half_vec = (light_vec + eye_vec).normalized();

        RGBColor ambient, diffuse, specular;
        ambient = ambient_light_intensity.cwiseProduct(ka);
        diffuse =
            kd.cwiseProduct(light_indensity) *
            std::max(0.0f, float(payload.normal.dot(light_vec.normalized())));
        // bling-phong specular shading model
        // https://www.cnblogs.com/bluebean/p/5299358.html
        specular =
            ks.cwiseProduct(light_indensity) *
            // negative fwd incident ray will be ignore
            std::pow(std::max(payload.normal.dot(half_vec), 0.0f), shiness);
        color = color + ambient + diffuse + specular;
    }

    return color.min(texture_color);
}
RGBColor Rasterizer::textureFragmentShaderNoLight(
    FragmentShaderPayload& payload) {
    if (!payload.texture) {
        return RGBColor(1.0);
    }
    RGBColor texture_color = payload.texture->getRGBBilinear(
        payload.texture_coord.x, payload.texture_coord.y);
    // coefficients for ambient, diffuse and specular lighting
    return texture_color;
}