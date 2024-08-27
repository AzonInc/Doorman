<script setup lang="ts">
const props = defineProps<{
  file: string
}>()

const { file } = toRefs(props);

import { toRefs } from 'vue'
import { useTresContext } from '@tresjs/core'

import { STLLoader } from 'three/examples/jsm/loaders/STLLoader';
import { Mesh, MeshBasicMaterial, MeshPhongMaterial } from 'three';

const loadSTLModel = (url: string): Promise<Mesh> => {
  const loader = new STLLoader();

  return new Promise((resolve, reject) => {
    loader.load(
      url,
      (geometry) => {
        const material = new MeshPhongMaterial({ color: 0xf7f7f7 });
        const meshWithMaterial = new Mesh(geometry, material);

        geometry.center();

        resolve(meshWithMaterial);
      },
      undefined, // onProgress callback (not used)
      (error) => reject(error) // onError callback
    );
  });
};

// Verwende async setup, um auf das Promise zu warten
const meshWithMaterial = await loadSTLModel(file.value);

const state = useTresContext()
state.invalidate()

</script>

<template>
  <primitive :object="meshWithMaterial" />
</template>