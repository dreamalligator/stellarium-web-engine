/* Stellarium Web Engine - Copyright (c) 2018 - Noctua Software Ltd
 *
 * This program is licensed under the terms of the GNU AGPL v3, or
 * alternatively under a commercial licence.
 *
 * The terms of the AGPL v3 license can be found in the main directory of this
 * repository.
 */

#include "projection.h"
#include "utils/vec.h"

/* Degrees to radians */
#define DD2R (1.745329251994329576923691e-2)

static void proj_perspective_project(
        const projection_t *proj, int flags, const double *v, double *out)
{
    double v4[4] = {v[0], v[1], v[2], 1};
    mat4_mul_vec4((void*)proj->mat, v4, out);
}

static bool proj_perspective_backward(const projection_t *proj, int flags,
        const double v[2], double out[4])
{
    double p[4] = {v[0], v[1], 0, 1};
    double inv[4][4];
    if (!mat4_invert(proj->mat, inv))
        assert(false);
    mat4_mul_vec4(inv, p, out);
    vec3_normalize(out, out);
    return true;
}

static void proj_perspective_compute_fov(int id, double fov, double aspect,
                                         double *fovx, double *fovy)
{
    if (aspect < 1) {
        *fovx = fov;
        *fovy = 2 * atan(tan(fov / 2) / aspect);
    } else {
        *fovy = fov;
        *fovx = 2 * atan(tan(fov / 2) * aspect);
    }
}

void proj_perspective_init(projection_t *p, double fov, double aspect)
{
    double fovy, clip_near = 0.1, clip_far = 256;
    fovy = atan(tan(fov / 2) / aspect) * 2;
    mat4_perspective(p->mat, fovy / DD2R, aspect, clip_near, clip_far);
    p->scaling[0] = tan(fov / 2);
    p->scaling[1] = p->scaling[0] / aspect;
}

static const projection_klass_t proj_perspective_klass = {
    .name           = "perspective",
    .id             = PROJ_PERSPECTIVE,
    .max_fov        = 180. * DD2R,
    .max_ui_fov     = 120. * DD2R,
    .init           = proj_perspective_init,
    .project        = proj_perspective_project,
    .backward       = proj_perspective_backward,
    .compute_fovs   = proj_perspective_compute_fov,
};
PROJECTION_REGISTER(proj_perspective_klass);
