import LabelledInput from "./LabelledInputComponent.js"
export default {
  setup() {

    const {inject} = Vue;

    const settings = inject('tank')
    async function reset () {
      try {
        console.log("reset")
        const response = await fetch("/api/tank/reset", {
          method: "PUT",
          headers: {
            "Content-Type": "application/json"
          },
          body: "{\"refill\": true}"
        })
        if (response.status === 204) {
          console.log("PUT successfully completed.")
        }
      } catch (error) {
        console.error("Fehler bei PUT: " , error)
      }
    }
    return {
      settings,
      reset
    }
  },
  template: 
  `<div> \
    <form @submit.prevent=""> \
      <fieldset class="grid-form label-box">\
        <legend>Tank</legend>\
        <labelled-input summary="Capacity" desc="The effective volume of the tank in milliliter."/>\
        <input id="Capacity" type="number" min='10' max='5000' v-model="settings.capacity" inputmode='dec' name="capacity"/>\
        ml\
        <labelled-input summary="Content" desc="The evaluated content of the tank in milliliter. The consumed volume is evaluated by number of pulses and consumption per puls."/>\
        <input id="Content" type="number" min='10' max='5000' v-model="settings.content" readonly="true" name="content" />\
        ml\
        <labelled-input summary="Reset" desc="Reset consumption. The content will be set to the tank capacity. Use after refilling the tank."/>\
        <button @click="reset" id="Reset" type="button" >Reset</button>\
        &nbsp;\
      </fieldset>\
    </form>\
  </div>`,
  components: {
    LabelledInput
  }
}