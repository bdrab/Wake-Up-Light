String webpageCode(int dimmer_light_level){

  String page_content = 
  R"(
<!DOCTYPE html>
    <html>
        <head>
            <style>
                body{
                    font-family: "Calibri"; 
                    background-color: grey
                }
                h1{
                    color: whitesmoke; 
                    text-align:center; 
                    font-size: 50px;
                }
                .content{
                    margin: 0;
                    position: absolute;
                    top: 50%;
                    left: 50%;
                    transform: translate(-50%, -50%);
                }
            </style>
        </head>      
        <body>
            <h1>Dimmer Light - Alarm <br><br></h1>
            <div class="content">  
                <div class="timerDiv">
                    <input type="time" id="timerTimeInput" required>
                    <input type="number" id="timerOffsetInput" placeholder="time in min">
                    <button class="setTimerBtn" type="button">Set Timer</button> 
                    <button class="unsetTimerBtn" type="button">Unset Timer</button> 

                </div>
                <div class="dimmerDiv">
                    <input type="range" min="10" max="100" value="10" step="5" class="slider" id="lightLevelRange">
                    <button class="turnOffBtn" type="button">Turn Off!</button> 
                </div>
            </div> 
            <script>
                const lightOffBtn = document.querySelector(".turnOffBtn");
         
                const timerObjInput = document.querySelector("#timerTimeInput");
                const timerSetBtn = document.querySelector(".setTimerBtn");
                const timerOffsetInput = document.querySelector("#timerOffsetInput");
                const timerUnsetBtn = document.querySelector(".unsetTimerBtn");


                const slider = document.querySelector("#lightLevelRange");

                const queryString = window.location.search;
                const urlParams = new URLSearchParams(queryString);
                const lightValue = urlParams.get("light");
  
                if(lightValue){
                    document.querySelector("#lightLevelRange").value = lightValue;
                }else{
                    document.querySelector("#lightLevelRange").value =
              )";
              page_content += String(dimmer_light_level);
      
    page_content += R"(
            }
  
                timerSetBtn.addEventListener("click", async event => {
                    
                    let data = timerObjInput.value.split(":");
                    let offsetValue = timerOffsetInput.value;
                    if (data[0] && data[1] && offsetValue){
                        let responseTime = await fetch('/set_alarm?' + new URLSearchParams({
                            hour: parseInt(data[0]),
                            minute: parseInt(data[1]),
                            offset: parseInt(offsetValue)
                        }))
                    }
                });
                
                timerUnsetBtn.addEventListener("click", async event => {
                  let responseTime = await fetch('/unset_alarm?')  
                });

                lightOffBtn.addEventListener('click', async function (event) {
                    let responseOff = await fetch('/light_off');
                    document.querySelector("#lightLevelRange").value = 11;
                });
  
                slider.addEventListener('input', async function (event) {
                    let responseSlider = await fetch('/set_light?' + new URLSearchParams({
                        light: this.value
                    }))
                });
            </script>
        </body>
    </html>
  )"; 
  
  return page_content;
}
