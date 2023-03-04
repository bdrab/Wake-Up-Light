String webpageCode(int dimmer_light_level, bool alarm_set_bool){

// Strings are probably not good for producing page content as it does not work good when page is very long. Sometimes page is not being displayed.  

  String page_content = 
  R"(
<!DOCTYPE html>
    <html>
        <head>
          <meta charset="UTF-8">
          <meta name="viewport" content="width=device-width, initial-scale=1" />

<style>
                body{
                    font-family: "Calibri"; 
                    background-color: gray
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
                    text-align: center;
                }
                
                .hide{
                  display: none;
                }

                #timerTimeInput{
                    width: 100%;
                    height: 60px;
                    text-align: center;
                    background-color: white;
                    border-radius: 60px;
                    margin-left: auto;
                    border: 0 solid #A9A9A9;
                    padding: 0;
                    margin-bottom: 20px;
                }

                #timerTimeInput:before {
                    content: 'TIME';
                    text-align: center;
                    margin: auto;
                }

                #timerOffsetInput{
                    width: 100%;
                    height: 60px;
                    text-align: center;
                    background-color: white;
                    border-radius: 60px;
                    margin-left: auto;
                    padding: 0;
                    border: 0 solid #A9A9A9;       
                }

                .buttonTimerDiv, .buttonDimmerDiv{
                    width: 60%;
                    height: 35px;
                    text-align: center;
                    background-color: white;
                    border-radius: 60px;
                    margin-left: auto;
                    border: 5px solid #A9A9A9;
                    padding: 0;
                    cursor: pointer;
                }

                #timerOffsetInput{
                    margin-bottom: 20px;
                }
                .timerDiv{
                    margin-bottom: 40px;
                }
                

                
            </style>
        </head>      
        <body>
            <h1>Dimmer Light - Alarm <br><br></h1>
            <div class="content">  
                <div class="timerDiv">
                    <input type="time" id="timerTimeInput" required><br>
                    <input type="number" id="timerOffsetInput" placeholder="time in min"><br>

                 )";

                 if(alarm_set_bool){
                  page_content += R"(
                    <button class="setTimerBtn buttonTimerDiv hide" type="button">Set Timer</button>
                    <button class="unsetTimerBtn buttonTimerDiv" type="button">Unset Timer</button> 
                  )";
                 }
                  else{
                    page_content += R"(
                      <button class="setTimerBtn buttonTimerDiv" type="button">Set Timer</button>
                      <button class="unsetTimerBtn buttonTimerDiv hide" type="button">Unset Timer</button>
                    )";
                    }
                   
                
  page_content += R"(
                </div>
                <div class="dimmerDiv">
                    <input type="range" min="10" max="100" value="10" step="5" class="slider" id="lightLevelRange"><br>
                    <button class="turnOffBtn buttonDimmerDiv" type="button">Turn Off!</button> 
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
                        let responseData = await responseTime.json();
                        if(responseData["status"] === "ok"){
                          timerSetBtn.classList.toggle("hide");
                          timerUnsetBtn.classList.toggle("hide");
                          alert("Alarm has been set!");
                          }
                        else{
                          alert("Alarm has not been set!");
                          }
                    }
                });
                
                timerUnsetBtn.addEventListener("click", async event => {
                  let responseTime = await fetch('/unset_alarm?');
                  let responseData = await responseTime.json();
                        if(responseData["status"] === "ok"){
                          timerSetBtn.classList.toggle("hide");
                          timerUnsetBtn.classList.toggle("hide");
                          alert("Alarm has been unset!");
                          }
                        else{
                          alert("Alarm has not been unset!");
                          }  
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
