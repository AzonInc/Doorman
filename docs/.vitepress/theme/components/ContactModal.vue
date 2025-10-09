<script setup lang="ts">
import { VPButton } from 'vitepress/theme'

const props = defineProps<{
  show: Boolean
}>()
</script>

<template>
  <Transition name="modal">
    <div v-if="show" class="modal-mask">
      <div class="modal-container">
        <div class="modal-content">
          <div class="modal-header">
            <slot name="header" />
          </div>
          <div class="modal-body">
            <slot name="body" />
          </div>
          <div class="modal-footer">
            <slot name="footer">
                <VPButton text="Close" @click="$emit('close')" />
            </slot>
          </div>
        </div>
      </div>
    </div>
  </Transition>
</template>

<style>
.modal-mask {
  position: fixed; /* changed from absolute */
  z-index: 9999;   /* ensure it's above everything */
  top: 0;
  left: 0;
  width: 100vw;    /* full viewport width */
  height: 100vh;   /* full viewport height */
  background-color: rgba(0,0,0,0.35); /* semi-transparent overlay */
  display: flex;
  align-items: center;
  justify-content: center;
  transition: opacity 0.3s ease;
}

.modal-container {
  display: flex;
  flex-direction: column;
  justify-content: center;
  align-items: center;
  margin: 0; /* remove auto margin for centering via flex */
  padding: 20px 30px;
  border-radius: 12px;
  border: 1px solid var(--vp-button-brand-active-bg);
  background-color: var(--vp-c-bg);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.33);
  transition: all 0.3s ease;
  max-width: 90vw;
  max-height: 90vh;
  overflow: auto;
}

.modal-content {
  display: block;
}

.modal-header h3 {
  margin-top: 0;
}

.modal-body {
  margin: 20px 0;
}

.modal-enter-from {
  opacity: 0;
}

.modal-leave-to {
  opacity: 0;
}

.modal-enter-from .modal-container,
.modal-leave-to .modal-container {
  -webkit-transform: scale(1.1);
  transform: scale(1.1);
}
</style>