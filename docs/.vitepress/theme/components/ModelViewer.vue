<script setup lang="ts">
const props = defineProps<{
  file: string
}>()

import { reactive, shallowRef, watch } from 'vue'
import { TresCanvas, useLoop, useRenderLoop  } from '@tresjs/core';
import { Box, OrbitControls, CameraControls } from '@tresjs/cientos'
import { BasicShadowMap, SRGBColorSpace, NoToneMapping } from 'three'

import STLFile from './STLLoader.vue';

const controlsState = reactive({
  minDistance: 0,
  maxDistance: 100,
  distance: 80
})

const gl = {
  clearColor: '#5c73e7',
  preset: "realistic",
  shadows: true,
  alpha: false,
  toneMapping: NoToneMapping,
}

const boxRef = shallowRef()

const { onLoop } = useRenderLoop()

onLoop(({ delta, elapsed }) => {
  if (boxRef.value) {
    boxRef.value.rotation.x = elapsed * 0.2
  }
})

</script>
<template>
  <TresCanvas v-bind="gl">
    <TresPerspectiveCamera />
    <CameraControls
      v-bind="controlsState"
      make-default
    />
    <Suspense>
      <TresMesh ref="boxRef">
        <STLFile receive-shadow ref="boxRef" v-bind:file="file" />
      </TresMesh>
    </Suspense>
    <TresAmbientLight :intensity="0.75" />
    <TresDirectionalLight
      :position="[4, 4, 6]"
      :intensity="1"
      cast-shadow
    />
    <TresDirectionalLight
      :position="[1, 6, 0]"
      :intensity="2"
      cast-shadow
    />
  </TresCanvas>
</template>