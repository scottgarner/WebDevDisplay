<template>
  <div class="min-h-screen h-full flex items-center justify-center">

    <div v-if="jsonData" class="w-full m-2">

      <form class="container m-auto p-2">

        <div v-if="statusMessage" class="text-white text-sm p-2 rounded bg-gray-600 mb-2">
          {{ statusMessage }}
        </div>

        <div v-for="(message, index) in jsonData.messages" :key="index" class="bg-zinc-100 rounded p-2 mb-2 relative">
          <label class="block text-gray-700 text-xs" for="text">
            text
          </label>
          <input class="appearance-none border w-full py-2 px-3 text-gray-700 leading-tight text-sm " id="text"
            type="text" v-model="message.text">

          <div class="grid grid-cols-2 gap-2 my-2" id="color">
            <div>
              <label for="speed">
                speed
              </label>
              <input class="appearance-none border w-full py-2 px-3 text-gray-700 leading-tight text-sm " id="speed"
                type="text" v-model="message.speed">
            </div>

            <div>
              <label for="duration">
                duration
              </label>
              <input class="appearance-none border w-full py-2 px-3 text-gray-700 leading-tight " id="duration"
                type="text" v-model="message.duration">
            </div>
          </div>

          <label for="color">
            color
          </label>
          <div class="grid grid-cols-4 gap-2 border-gray-200 bg-white border rounded p-2 text-sm" id="color">

            <div class="col-span-3 m-0 p-0 h-full">
              <input type="range" id="red" v-model="message.red" min="0" max="255"
                class="w-full appearance-none bg-red-400 rounded p-0 m-0 h-2" />
              <input type="range" id="green" v-model="message.green" min="0" max="255"
                class="w-full appearance-none bg-green-400 rounded p-0 m-0 h-2" />
              <input type="range" id="blue" v-model="message.blue" min="0" max="255"
                class="w-full appearance-none bg-blue-400 rounded p-0 m-0 h-2 " />
            </div>

            <div class="border border-gray rounded w-full h-full" :style="{ backgroundColor: rgbColor(index) }"></div>
          </div>

          <button v-if="index > 0" class="absolute top-0 right-0 bg-red-500 active:bg-red-700 text-white px-2 py-1 -m-1"
            type="button" @click="removeMessage(index)">X
          </button>
        </div>

        <div class="grid grid-cols-2 gap-2">
          <button class="bg-orange-500 active:bg-orange-700 text-white  py-2 px-4 " type="button" @click="submit">
            Submit
          </button>

          <button v-if="jsonData.messages.length < 4" class="bg-blue-500 active:bg-blue-700 text-white py-2 px-4 "
            type="button" @click="addMessage">
            Add Message
          </button>
        </div>

      </form>
    </div>


    <div v-if="!jsonData" class="text-white">
      Loading...
    </div>

  </div>

</template>

<script setup lang="ts">

import { ref, onMounted, computed } from 'vue';

const jsonData = ref<any>(null);
const statusMessage = ref<string | null>(null);

let timeout: number;


onMounted(async () => {
  try {
    const response = await fetch('http://scoreboard.local/data');
    jsonData.value = await response.json();

    console.log(jsonData.value);
  } catch (error) {
    console.error('Error fetching data:', error);
  }
});

const addMessage = () => {
  jsonData.value.messages.push({ text: 'HIGH ORDER', speed: '8', duration: '8', red: 255, green: 255, blue: 255 });
};

const removeMessage = (index: number) => {
  jsonData.value.messages.splice(index, 1);
};

const rgbColor = (index: number) => {
  var message = jsonData.value.messages[index];
  return `rgb(${message.red}, ${message.green}, ${message.blue})`;
};

const submit = async () => {
  try {
    statusMessage.value = "";

    const body = JSON.stringify(jsonData.value);
    const response = await fetch('http://scoreboard.local/data', {
      method: 'POST',
      headers: {
        'Content-Type': 'text/plain',
      },
      body,
    });

    console.log('Data posted successfully:', response);
    statusMessage.value = 'Data submitted successfully...';
  } catch (error: any) {
    console.error('Error posting data:', error);
    statusMessage.value = `Error submitting data!`;
  }

  clearTimeout(timeout);

  timeout = setTimeout(() => {
    statusMessage.value = "";
  }, 1000);

};

</script>
