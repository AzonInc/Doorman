<script setup lang="ts">
import { VPButton } from 'vitepress/theme'
import { onMounted, onUnmounted } from 'vue'

const props = defineProps<{
  show: Boolean
}>()

const emit = defineEmits<{
  close: []
}>()

function onKeydown(e: KeyboardEvent) {
  if (props.show && e.key === 'Escape') {
    emit('close')
  }
}

onMounted(() => window.addEventListener('keydown', onKeydown))
onUnmounted(() => window.removeEventListener('keydown', onKeydown))
</script>

<template>
  <Transition name="modal">
    <div v-if="show" class="modal-mask" @click.self="$emit('close')">
      <div class="modal-container">
        <div class="modal-content">
          <div class="modal-header">
            <slot name="header" />
            <button class="close" @click="$emit('close')">&times;</button>
          </div>
          <div class="modal-body">
            <slot name="body" />
          </div>
          <div class="modal-footer">
            <slot name="footer" />
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
  margin: 0; /* remove auto margin for centering via flex */
  border-radius: 12px;
  border: 1px solid var(--vp-button-brand-active-bg);
  background-color: var(--vp-c-bg);
  box-shadow: 0 2px 8px rgba(0, 0, 0, 0.33);
  transition: all 0.3s ease;
  max-width: 90vw;
  max-height: 90vh;
  min-width: 400px;
  overflow: hidden;
}

.modal-content {
  display: flex;
  flex-direction: column;
  width: 100%;
  min-height: 0;
}

.modal-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  border-bottom: 1px solid var(--vp-c-divider);
  padding: 20px;
  flex-shrink: 0;
}

.modal-header .close {
    height: 35px;
    width: 35px;
    font-size: 23px;
    line-height: 23px;
}

.modal-header h3 {
  margin-top: 0;
}

.modal-body {
  padding: 20px;
  flex: 1 1 auto;
  overflow-y: auto;
  min-height: 0;
}

.modal-footer {
  padding: 20px;
  display: flex;
  justify-content: center;
  gap: 15px;
  flex-shrink: 0;
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