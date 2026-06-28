import { computed, type MaybeRefOrGetter, toValue } from "vue";
import { GLTFLoader } from "three/addons/loaders/GLTFLoader.js";
import { DDSLoader } from "three/examples/jsm/loaders/DDSLoader.js";
import { getModManUrl } from "@/meta/ModManUrl.ts";
import { Loader, LoadingManager } from "three";
export { type GLTF } from "three/addons/loaders/GLTFLoader.js";

const IMAGE_REGEX = /\.\.[\\/]images[\\/](.+)\.(.+)$/m;
class ProxyImageLoader extends Loader {
  private ddsLoader: DDSLoader;
  private readonly zoneName: string;

  constructor(zoneName: string) {
    super();
    this.ddsLoader = new DDSLoader();
    this.zoneName = zoneName;
  }
  load(
    url: string,
    onLoad: (data: unknown) => void,
    onProgress?: (event: ProgressEvent) => void,
    onError?: (err: unknown) => void,
  ) {
    const match = IMAGE_REGEX.exec(url);
    if (!match) {
      onError?.("invalid url");
      return;
    }

    this.ddsLoader.load(
      getModManUrl(
        `/image/dds?zone=${encodeURIComponent(this.zoneName)}&name=${encodeURIComponent(match[1])}`,
      ),
      onLoad,
      onProgress,
      onError,
    );
  }
  loadAsync(url: string, onProgress?: (event: ProgressEvent) => void): Promise<unknown> {
    const match = IMAGE_REGEX.exec(url);
    if (!match) {
      return Promise.reject("invalid url");
    }

    return this.ddsLoader.loadAsync(
      getModManUrl(
        `/image/dds?zone=${encodeURIComponent(this.zoneName)}&name=${encodeURIComponent(match[1])}`,
      ),
      onProgress,
    );
  }
}

export function useGltfLoader(zoneName: MaybeRefOrGetter<string>) {
  return computed(() => {
    const manager = new LoadingManager();
    manager.addHandler(IMAGE_REGEX, new ProxyImageLoader(toValue(zoneName)));

    return new GLTFLoader(manager);
  });
}
