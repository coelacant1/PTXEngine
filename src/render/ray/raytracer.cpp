#include "../../../include/um3d/render/ray/raytracer.hpp"

Quaternion RayTracer::rayDirection;

RGBColor RayTracer::RayTracePixel(Triangle2D** triangles, int numTriangles, Vector2D pixelRay) {
    float zBuffer = 3.402823466e+38f;
    int triangle = 0;
    bool didIntersect = false;
    float u = 0.0f, v = 0.0f, w = 0.0f;
    Vector3D uvw;
    RGBColor color;

    for (int t = 0; t < numTriangles; ++t) {
        if (triangles[t]->averageDepth < zBuffer) {
            if (triangles[t]->DidIntersect(pixelRay.X, pixelRay.Y, u, v, w)) {
                uvw.X = u;
                uvw.Y = v;
                uvw.Z = w;
                zBuffer = triangles[t]->averageDepth;
                triangle = t;
                didIntersect = true;
            }
        }
    }
    
    if (didIntersect) {
        Vector3D intersect = (*triangles[triangle]->t3p1 * uvw.X) + (*triangles[triangle]->t3p2 * uvw.Y) + (*triangles[triangle]->t3p3 * uvw.Z);
        Vector2D uv;

        if (triangles[triangle]->hasUV) uv = *triangles[triangle]->p1UV * uvw.X + *triangles[triangle]->p2UV * uvw.Y + *triangles[triangle]->p3UV * uvw.Z;

        color = triangles[triangle]->GetMaterial()->GetRGB(intersect, *triangles[triangle]->normal, Vector3D(uv.X, uv.Y, 0.0f));
    }

    return color;
}

void RayTracer::RayTrace(Scene* scene, CameraBase* camera) {
    if (camera->Is2D()) {
        for (unsigned int i = 0; i < camera->GetPixelGroup()->GetPixelCount(); i++) {
            Vector2D pixelRay = camera->GetPixelGroup()->GetCoordinate(i);
            Vector3D pixelRay3D = Vector3D(pixelRay.X, pixelRay.Y, 0) + camera->GetTransform()->GetPosition();

            RGBColor color = scene->GetObjects()[0]->GetMaterial()->GetRGB(pixelRay3D, Vector3D(), Vector3D());

            camera->GetPixelGroup()->GetColor(i)->R = color.R;
            camera->GetPixelGroup()->GetColor(i)->G = color.G;
            camera->GetPixelGroup()->GetColor(i)->B = color.B;
        }
    } else {
        camera->GetTransform()->SetBaseRotation(camera->GetCameraLayout()->GetRotation());//Set camera layout

        rayDirection = camera->GetTransform()->GetRotation().Multiply(camera->GetLookOffset());//Apply offset to camera if set

        //Set quad tree space to local camera coordinates
        Vector2D minCoord = camera->GetCameraMinCoordinate();
        Vector2D maxCoord = camera->GetCameraMaxCoordinate();

        QuadTree tree = QuadTree(minCoord, maxCoord);

        uint16_t tCount = 1;

        for (uint8_t i = 0; i < scene->GetObjectCount(); ++i) {
            if (scene->GetObjects()[i]->IsEnabled()) {
                tCount += scene->GetObjects()[i]->GetTriangleGroup()->GetTriangleCount();
            }
        }

        Triangle2D triangles[tCount];
        uint16_t iterCount = 0;

        for (uint8_t i = 0; i < scene->GetObjectCount(); ++i) {
            if (scene->GetObjects()[i]->IsEnabled()) {
                for (uint16_t j = 0; j < scene->GetObjects()[i]->GetTriangleGroup()->GetTriangleCount(); ++j) {
                    //Create 2D triangle mapping for rasterize, stores 3D coordinates for mapping material to 3d global coordinate space
                    triangles[iterCount] = Triangle2D(camera->GetLookOffset(), camera->GetTransform(), &scene->GetObjects()[i]->GetTriangleGroup()->GetTriangles()[j], scene->GetObjects()[i]->GetMaterial());
                    
                    tree.Insert(&triangles[iterCount]);
                    iterCount++;
                }
            }
        }

        tree.Rebuild();

        Vector2D scale, pixelRay, materialRay;
        for (uint16_t i = 0; i < camera->GetPixelGroup()->GetPixelCount(); ++i) {
            //Render camera in local camera space
            pixelRay = camera->GetPixelGroup()->GetCoordinate(i);
            
            Node* leafNode = tree.Intersect(pixelRay);

            if (!leafNode || leafNode->GetCount() == 0) {
                camera->GetPixelGroup()->GetColor(i)->R = 0;
                camera->GetPixelGroup()->GetColor(i)->G = 0;
                camera->GetPixelGroup()->GetColor(i)->B = 0;
                continue;
            }

            //Render individual pixel, transformed to camera coordinate space
            RGBColor color = CheckRasterPixel(leafNode->GetEntities(), leafNode->GetCount(), pixelRay);

            camera->GetPixelGroup()->GetColor(i)->R = color.R;
            camera->GetPixelGroup()->GetColor(i)->G = color.G;
            camera->GetPixelGroup()->GetColor(i)->B = color.B;
        }
    }
}
