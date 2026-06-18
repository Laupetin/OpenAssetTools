import { BufferGeometry, Material, Object3D, Texture } from "three";
import { type IUniform } from "three/src/renderers/shaders/UniformsLib.js";

function getMaterialsOfObject(object: Object3D) {
  if ("material" in object) {
    const value = object.material as Material | Material[];
    return Array.isArray(value) ? value : [value];
  }

  return [];
}

export class ThreeResourceTracker {
  private readonly textures: Record<number, number>;
  private readonly materials: Record<string, number>;
  private readonly geometries: Record<number, number>;
  private readonly objects: Record<number, number>;

  constructor() {
    this.textures = {};
    this.materials = {};
    this.geometries = {};
    this.objects = {};
  }

  refTexture(texture: Texture) {
    if (!this.textures[texture.id]) {
      this.textures[texture.id] = 1;
    } else {
      this.textures[texture.id]++;
    }
  }

  refMaterial(material: Material) {
    if (!this.materials[material.uuid]) {
      this.materials[material.uuid] = 1;
      for (const property of Object.values(material)) {
        if (property instanceof Texture) {
          this.refTexture(property);
        }
        if ("uniforms" in material) {
          for (const value of Object.values(material.uniforms as Record<string, IUniform>)) {
            if (value) {
              const uniformValue = value.value;
              const uniformValues = Array.isArray(uniformValue) ? uniformValue : [uniformValue];

              for (const maybeTexture of uniformValues) {
                if (maybeTexture instanceof Texture) {
                  this.refTexture(maybeTexture);
                }
              }
            }
          }
        }
      }
    } else {
      this.materials[material.uuid]++;
    }
  }

  refGeometry(geometry: BufferGeometry) {
    if (!this.geometries[geometry.id]) {
      this.geometries[geometry.id] = 1;
    } else {
      this.geometries[geometry.id]++;
    }
  }

  refObject(object: Object3D) {
    if (!this.objects[object.id]) {
      this.objects[object.id] = 1;
      for (const material of getMaterialsOfObject(object)) {
        this.refMaterial(material);
      }
      if ("geometry" in object) {
        this.refGeometry(object.geometry as BufferGeometry);
      }
      for (const child of object.children) {
        this.refObject(child);
      }
    } else {
      this.objects[object.id]++;
    }
  }

  unrefTexture(texture: Texture) {
    const refCount = this.textures[texture.id];
    if (refCount === undefined) {
      return;
    }

    if (refCount > 1) {
      this.textures[texture.id] = refCount - 1;
    } else {
      delete this.textures[texture.id];
      texture.dispose();
    }
  }

  unrefMaterial(material: Material) {
    const refCount = this.materials[material.uuid];
    if (refCount === undefined) {
      return;
    }

    if (refCount > 1) {
      this.materials[material.uuid] = refCount - 1;
    } else {
      for (const property of Object.values(material)) {
        if (property instanceof Texture) {
          this.unrefTexture(property);
        }
        if ("uniforms" in material) {
          for (const value of Object.values(material.uniforms as Record<string, IUniform>)) {
            if (value) {
              const uniformValue = value.value;
              const uniformValues = Array.isArray(uniformValue) ? uniformValue : [uniformValue];

              for (const maybeTexture of uniformValues) {
                if (maybeTexture instanceof Texture) {
                  this.unrefTexture(maybeTexture);
                }
              }
            }
          }
        }
      }
      material.dispose();
      delete this.materials[material.uuid];
    }
  }

  unrefGeometry(geometry: BufferGeometry) {
    const refCount = this.geometries[geometry.id];
    if (refCount === undefined) {
      return;
    }

    if (refCount > 1) {
      this.geometries[geometry.id] = refCount - 1;
    } else {
      delete this.geometries[geometry.id];
      geometry.dispose();
    }
  }

  unrefObject(object: Object3D) {
    const refCount = this.objects[object.id];
    if (refCount === undefined) {
      return;
    }

    if (refCount > 1) {
      this.objects[object.id] = refCount - 1;
    } else {
      for (const material of getMaterialsOfObject(object)) {
        this.unrefMaterial(material);
      }
      if ("geometry" in object) {
        this.unrefGeometry(object.geometry as BufferGeometry);
      }
      for (const child of object.children) {
        this.unrefObject(child);
      }

      delete this.objects[object.id];
    }
  }
}
