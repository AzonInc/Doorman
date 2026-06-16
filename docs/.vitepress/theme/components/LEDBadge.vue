<script setup lang="ts">
const props = defineProps<{
  color: string
  type?: 'pulse-slow' | 'pulse-fast' | 'pulse-error' | 'solid' | 'solid-fade-out'
  title?: string
}>()
</script>

<template>
  <span
    class="led-badge"
    :class="`led-${type ?? 'pulse-slow'}`"
    :style="{ '--led-color': color }"
    :title="title"
  />
</template>

<style>
.led-badge {
  display: inline-block;
  width: 10px;
  height: 10px;
  border-radius: 50%;
  background-color: var(--led-color);
  vertical-align: middle;
  margin: 0 2px;
  flex-shrink: 0;
}

.led-pulse-slow {
  animation: led-pulse-slow 4s ease-in-out infinite;
}

.led-pulse-fast {
  animation: led-pulse-fast 0.6s ease-in-out infinite;
}

.led-pulse-error {
  animation: led-pulse-error 1.2s ease-in-out infinite;
}

.led-solid-fade-out {
  animation: led-solid-fade-out 6s infinite;
}

@keyframes led-solid-fade-out {
  0% {
    opacity: 0;
  }

  /* harter Sprung auf sichtbar */
  1% {
    opacity: 1;
  }

  /* 3 Sekunden sichtbar bleiben */
  65% {
    opacity: 1;
  }

  85% {
    opacity: 0;
  }

  /* weich ausfaden */
  100% {
    opacity: 0;
  }
}

.led-solid {
  opacity: 1;
}

@keyframes led-pulse-slow {
  0%, 20%, 80%, 100% { opacity: 1; }
  50%      { opacity: 0.25; }
}

@keyframes led-pulse-fast {
  0%, 100% { opacity: 1; }
  50%      { opacity: 0.15; }
}

@keyframes led-pulse-error {
  0%, 100% { background-color: var(--led-color); }
  50%      { background-color: #FF0000; }
}
</style>