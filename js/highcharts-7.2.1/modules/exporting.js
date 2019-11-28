(function(a){"object"===typeof module&&module.exports?(a["default"]=a,module.exports=a):"function"===typeof define&&define.amd?define("highcharts/modules/exporting",["highcharts"],function(b){a(b);a.Highcharts=b;return a}):a("undefined"!==typeof Highcharts?Highcharts:void 0)})(function(b){function a(d,i,h,c){d.hasOwnProperty(i)||(d[i]=c.apply(null,h))}b=b?b._modules:{};a(b,"modules/full-screen.src.js",[b["parts/Globals.js"]],function(c){(c.FullScreen=function(d){this.init(d.parentNode)}).prototype={init:function(f){var d;f.requestFullscreen?d=f.requestFullscreen():f.mozRequestFullScreen?d=f.mozRequestFullScreen():f.webkitRequestFullscreen?d=f.webkitRequestFullscreen():f.msRequestFullscreen&&(d=f.msRequestFullscreen());if(d){d["catch"](function(){alert("Full screen is not supported inside a frame")})}}}});a(b,"mixins/navigation.js",[],function(){return{initUpdate:function(c){c.navigation||(c.navigation={updates:[],update:function(d,e){this.updates.forEach(function(f){f.update.call(f.context,d,e)})}})},addUpdate:function(c,d){d.navigation||this.initUpdate(d);d.navigation.updates.push({update:c,context:d})}}});a(b,"modules/exporting.src.js",[b["parts/Globals.js"],b["parts/Utilities.js"],b["mixins/navigation.js"]],function(T,V,U){var S=V.discardElement,z=V.extend,l=V.isObject,H=V.objectEach,N=V.pick;V=T.defaultOptions;var o=T.doc,P=T.Chart,m=T.addEvent,j=T.removeEvent,O=T.fireEvent,s=T.createElement,R=T.css,Q=T.merge,h=T.isTouchDevice,i=T.win,u=i.navigator.userAgent,E=T.SVGRenderer,q=T.Renderer.prototype.symbols,d=/Edge\/|Trident\/|MSIE /.test(u),c=/firefox/i.test(u);z(V.lang,{viewFullscreen:"View in full screen",printChart:"Print chart",downloadPNG:"Download PNG image",downloadJPEG:"Download JPEG image",downloadPDF:"Download PDF document",downloadSVG:"Download SVG vector image",contextButtonTitle:"Chart context menu"});V.navigation||(V.navigation={});Q(!0,V.navigation,{buttonOptions:{theme:{},symbolSize:14,symbolX:12.5,symbolY:10.5,align:"right",buttonSpacing:3,height:22,verticalAlign:"top",width:24}});Q(!0,V.navigation,{menuStyle:{border:"1px solid #999999",background:"#ffffff",padding:"5px 0"},menuItemStyle:{padding:"0.5em 1em",color:"#333333",background:"none",fontSize:h?"14px":"11px",transition:"background 250ms, color 250ms"},menuItemHoverStyle:{background:"#335cad",color:"#ffffff"},buttonOptions:{symbolFill:"#666666",symbolStroke:"#666666",symbolStrokeWidth:3,theme:{padding:5}}});V.exporting={type:"image/png",url:"https://export.highcharts.com/",printMaxWidth:780,scale:2,buttons:{contextButton:{className:"highcharts-contextbutton",menuClassName:"highcharts-contextmenu",symbol:"menu",titleKey:"contextButtonTitle",menuItems:"viewFullscreen printChart separator downloadPNG downloadJPEG downloadPDF downloadSVG".split(" ")}},menuItemDefinitions:{viewFullscreen:{textKey:"viewFullscreen",onclick:function(){this.fullscreen=new T.FullScreen(this.container)}},printChart:{textKey:"printChart",onclick:function(){this.print()}},separator:{separator:!0},downloadPNG:{textKey:"downloadPNG",onclick:function(){this.exportChart()}},downloadJPEG:{textKey:"downloadJPEG",onclick:function(){this.exportChart({type:"image/jpeg"})}},downloadPDF:{textKey:"downloadPDF",onclick:function(){this.exportChart({type:"application/pdf"})}},downloadSVG:{textKey:"downloadSVG",onclick:function(){this.exportChart({type:"image/svg+xml"})}}}};T.post=function(f,e,k){var g=s("form",Q({method:"post",action:f,enctype:"multipart/form-data"},k),{display:"none"},o.body);H(e,function(n,p){s("input",{type:"hidden",name:p,value:n},null,g)});g.submit();S(g)};z(P.prototype,{sanitizeSVG:function(f,e){var k=f.indexOf("</svg>")+6,g=f.substr(k);f=f.substr(0,k);e&&e.exporting&&e.exporting.allowHTML&&g&&(g='<foreignObject x="0" y="0" width="'+e.chart.width+'" height="'+e.chart.height+'"><body xmlns="http://www.w3.org/1999/xhtml">'+g+"</body></foreignObject>",f=f.replace("</svg>",g+"</svg>"));f=f.replace(/zIndex="[^"]+"/g,"").replace(/symbolName="[^"]+"/g,"").replace(/jQuery[0-9]+="[^"]+"/g,"").replace(/url\(("|&quot;)(.*?)("|&quot;);?\)/g,"url($2)").replace(/url\([^#]+#/g,"url(#").replace(/<svg /,'<svg xmlns:xlink="http://www.w3.org/1999/xlink" ').replace(/ (|NS[0-9]+:)href=/g," xlink:href=").replace(/\n/," ").replace(/(fill|stroke)="rgba\(([ 0-9]+,[ 0-9]+,[ 0-9]+),([ 0-9\.]+)\)"/g,'$1="rgb($2)" $1-opacity="$3"').replace(/&nbsp;/g,"\u00a0").replace(/&shy;/g,"\u00ad");this.ieSanitizeSVG&&(f=this.ieSanitizeSVG(f));return f},getChartHTML:function(){this.styledMode&&this.inlineStyles();return this.container.innerHTML},getSVG:function(k){var g,v=Q(this.options,k);v.plotOptions=Q(this.userOptions.plotOptions,k&&k.plotOptions);v.time=Q(this.userOptions.time,k&&k.time);var t=s("div",null,{position:"absolute",top:"-9999em",width:this.chartWidth+"px",height:this.chartHeight+"px"},o.body);var r=this.renderTo.style.width;var p=this.renderTo.style.height;r=v.exporting.sourceWidth||v.chart.width||/px$/.test(r)&&parseInt(r,10)||(v.isGantt?800:600);p=v.exporting.sourceHeight||v.chart.height||/px$/.test(p)&&parseInt(p,10)||400;z(v.chart,{animation:!1,renderTo:t,forExport:!0,renderer:"SVGRenderer",width:r,height:p});v.exporting.enabled=!1;delete v.data;v.series=[];this.series.forEach(function(e){g=Q(e.userOptions,{animation:!1,enableMouseTracking:!1,showCheckbox:!1,visible:e.visible});g.isInternal||v.series.push(g)});this.axes.forEach(function(e){e.userOptions.internalKey||(e.userOptions.internalKey=T.uniqueKey())});var n=new T.Chart(v,this.callback);k&&["xAxis","yAxis","series"].forEach(function(e){var f={};k[e]&&(f[e]=k[e],n.update(f))});this.axes.forEach(function(e){var f=T.find(n.axes,function(y){return y.options.internalKey===e.userOptions.internalKey}),w=e.getExtremes(),x=w.userMin;w=w.userMax;f&&(void 0!==x&&x!==f.min||void 0!==w&&w!==f.max)&&f.setExtremes(x,w,!0,!1)});r=n.getChartHTML();O(this,"getSVG",{chartCopy:n});r=this.sanitizeSVG(r,v);v=null;n.destroy();S(t);return r},getSVGForExport:function(f,e){var g=this.options.exporting;return this.getSVG(Q({chart:{borderRadius:0}},g.chartOptions,e,{exporting:{sourceWidth:f&&f.sourceWidth||g.sourceWidth,sourceHeight:f&&f.sourceHeight||g.sourceHeight}}))},getFilename:function(){var f=this.userOptions.title&&this.userOptions.title.text,e=this.options.exporting.filename;if(e){return e}"string"===typeof f&&(e=f.toLowerCase().replace(/<\/?[^>]+(>|$)/g,"").replace(/[\s_]+/g,"-").replace(/[^a-z0-9\-]/g,"").replace(/^[\-]+/g,"").replace(/[\-]+/g,"-").substr(0,24).replace(/[\-]+$/g,""));if(!e||5>e.length){e="chart"}return e},exportChart:function(f,e){e=this.getSVGForExport(f,e);f=Q(this.options.exporting,f);T.post(f.url,{filename:f.filename||this.getFilename(),type:f.type,width:f.width||0,scale:f.scale,svg:e},f.formAttributes)},print:function(){function n(e){(k.fixedDiv?[k.fixedDiv,k.scrollingContainer]:[k.container]).forEach(function(f){e.appendChild(f)})}var k=this,x=[],w=o.body,v=w.childNodes,r=k.options.exporting.printMaxWidth,t;if(!k.isPrinting){k.isPrinting=!0;k.pointer.reset(null,0);O(k,"beforePrint");if(t=r&&k.chartWidth>r){var p=[k.options.chart.width,void 0,!1];k.setSize(r,void 0,!1)}[].forEach.call(v,function(f,e){1===f.nodeType&&(x[e]=f.style.display,f.style.display="none")});n(w);setTimeout(function(){i.focus();i.print();setTimeout(function(){n(k.renderTo);[].forEach.call(v,function(f,e){1===f.nodeType&&(f.style.display=x[e]||"")});k.isPrinting=!1;t&&k.setSize.apply(k,p);O(k,"afterPrint")},1000)},1)}},contextMenu:function(F,D,C,A,y,x,I){var w=this,g=w.options.navigation,n=w.chartWidth,B=w.chartHeight,r="cache-"+F,t=w[r],G=Math.max(y,x);if(!t){w.exportContextMenu=w[r]=t=s("div",{className:F},{position:"absolute",zIndex:1000,padding:G+"px",pointerEvents:"auto"},w.fixedDiv||w.container);var v=s("div",{className:"highcharts-menu"},null,t);w.styledMode||R(v,z({MozBoxShadow:"3px 3px 10px #888",WebkitBoxShadow:"3px 3px 10px #888",boxShadow:"3px 3px 10px #888"},g.menuStyle));t.hideMenu=function(){R(t,{display:"none"});I&&I.setState(0);w.openMenu=!1;R(w.renderTo,{overflow:"hidden"});T.clearTimeout(t.hideTimer);O(w,"exportMenuHidden")};w.exportEvents.push(m(t,"mouseleave",function(){t.hideTimer=i.setTimeout(t.hideMenu,500)}),m(t,"mouseenter",function(){T.clearTimeout(t.hideTimer)}),m(o,"mouseup",function(e){w.pointer.inClass(e.target,F)||t.hideMenu()}),m(t,"click",function(){w.openMenu&&t.hideMenu()}));D.forEach(function(f){"string"===typeof f&&(f=w.options.exporting.menuItemDefinitions[f]);if(l(f,!0)){if(f.separator){var e=s("hr",null,null,v)}else{e=s("div",{className:"highcharts-menu-item",onclick:function(k){k&&k.stopPropagation();t.hideMenu();f.onclick&&f.onclick.apply(w,arguments)},innerHTML:f.text||w.options.lang[f.textKey]},null,v),w.styledMode||(e.onmouseover=function(){R(this,g.menuItemHoverStyle)},e.onmouseout=function(){R(this,g.menuItemStyle)},R(e,z({cursor:"pointer"},g.menuItemStyle)))}w.exportDivElements.push(e)}});w.exportDivElements.push(v,t);w.exportMenuWidth=t.offsetWidth;w.exportMenuHeight=t.offsetHeight}D={display:"block"};C+w.exportMenuWidth>n?D.right=n-C-y-G+"px":D.left=C-G+"px";A+x+w.exportMenuHeight>B&&"top"!==I.alignOptions.verticalAlign?D.bottom=B-A-G+"px":D.top=A+x-G+"px";R(t,D);R(w.renderTo,{overflow:""});w.openMenu=!0;O(w,"exportMenuShown")},addButton:function(G){var F=this,D=F.renderer,B=Q(F.options.navigation.buttonOptions,G),A=B.onclick,x=B.menuItems,y=B.symbolSize||12;F.btnCount||(F.btnCount=0);F.exportDivElements||(F.exportDivElements=[],F.exportSVGElements=[]);if(!1!==B.enabled){var w=B.theme,v=w.states,p=v&&v.hover;v=v&&v.select;var C;F.styledMode||(w.fill=N(w.fill,"#ffffff"),w.stroke=N(w.stroke,"none"));delete w.states;A?C=function(e){e&&e.stopPropagation();A.call(F,e)}:x&&(C=function(e){e&&e.stopPropagation();F.contextMenu(r.menuClassName,x,r.translateX,r.translateY,r.width,r.height,r);r.setState(2)});B.text&&B.symbol?w.paddingLeft=N(w.paddingLeft,25):B.text||z(w,{width:B.width,height:B.height,padding:0});F.styledMode||(w["stroke-linecap"]="round",w.fill=N(w.fill,"#ffffff"),w.stroke=N(w.stroke,"none"));var r=D.button(B.text,0,0,C,w,p,v).addClass(G.className).attr({title:N(F.options.lang[B._titleKey||B.titleKey],"")});r.menuClassName=G.menuClassName||"highcharts-menu-"+F.btnCount++;if(B.symbol){var t=D.symbol(B.symbol,B.symbolX-y/2,B.symbolY-y/2,y,y,{width:y,height:y}).addClass("highcharts-button-symbol").attr({zIndex:1}).add(r);F.styledMode||t.attr({stroke:B.symbolStroke,fill:B.symbolFill,"stroke-width":B.symbolStrokeWidth||1})}r.add(F.exportingGroup).align(z(B,{width:r.width,x:N(B.x,F.buttonOffset)}),!0,"spacingBox");F.buttonOffset+=(r.width+B.buttonSpacing)*("right"===B.align?-1:1);F.exportSVGElements.push(r,t)}},destroyExport:function(g){var f=g?g.target:this;g=f.exportSVGElements;var p=f.exportDivElements,n=f.exportEvents,k;g&&(g.forEach(function(e,r){e&&(e.onclick=e.ontouchstart=null,k="cache-"+e.menuClassName,f[k]&&delete f[k],f.exportSVGElements[r]=e.destroy())}),g.length=0);f.exportingGroup&&(f.exportingGroup.destroy(),delete f.exportingGroup);p&&(p.forEach(function(e,r){T.clearTimeout(e.hideTimer);j(e,"mouseleave");f.exportDivElements[r]=e.onmouseout=e.onmouseover=e.ontouchstart=e.onclick=null;S(e)}),p.length=0);n&&(n.forEach(function(e){e()}),n.length=0)}});E.prototype.inlineToAttributes="fill stroke strokeLinecap strokeLinejoin strokeWidth textAnchor x y".split(" ");E.prototype.inlineBlacklist=[/-/,/^(clipPath|cssText|d|height|width)$/,/^font$/,/[lL]ogical(Width|Height)$/,/perspective/,/TapHighlightColor/,/^transition/,/^length$/];E.prototype.unstyledElements=["clipPath","defs","desc"];P.prototype.inlineStyles=function(){function C(e){return e.replace(/([A-Z])/g,function(g,f){return"-"+f.toLowerCase()})}function B(r){function g(K,L){I=f=!1;if(v){for(e=v.length;e--&&!f;){f=v[e].test(L)}I=!f}"transform"===L&&"none"===K&&(I=!0);for(e=x.length;e--&&!I;){I=x[e].test(L)||"function"===typeof K}I||D[L]===K&&"svg"!==r.nodeName||t[r.nodeName][L]===K||(-1!==y.indexOf(L)?r.setAttribute(C(L),K):k+=C(L)+":"+K+";")}var k="",I,f,e;if(1===r.nodeType&&-1===w.indexOf(r.nodeName)){var J=i.getComputedStyle(r,null);var D="svg"===r.nodeName?{}:i.getComputedStyle(r.parentNode,null);if(!t[r.nodeName]){p=n.getElementsByTagName("svg")[0];var G=n.createElementNS(r.namespaceURI,r.nodeName);p.appendChild(G);t[r.nodeName]=Q(i.getComputedStyle(G,null));"text"===r.nodeName&&delete t.text.fill;p.removeChild(G)}if(c||d){for(var F in J){g(J[F],F)}}else{H(J,g)}k&&(J=r.getAttribute("style"),r.setAttribute("style",(J?J+";":"")+k));"svg"===r.nodeName&&r.setAttribute("stroke-width","1px");"text"!==r.nodeName&&[].forEach.call(r.children||r.childNodes,B)}}var A=this.renderer,y=A.inlineToAttributes,x=A.inlineBlacklist,v=A.inlineWhitelist,w=A.unstyledElements,t={},p;A=o.createElement("iframe");R(A,{width:"1px",height:"1px",visibility:"hidden"});o.body.appendChild(A);var n=A.contentWindow.document;n.open();n.write('<svg xmlns="http://www.w3.org/2000/svg"></svg>');n.close();B(this.container.querySelector("svg"));p.parentNode.removeChild(p)};q.menu=function(f,e,k,g){return["M",f,e+2.5,"L",f+k,e+2.5,"M",f,e+g/2+0.5,"L",f+k,e+g/2+0.5,"M",f,e+g-1.5,"L",f+k,e+g-1.5]};q.menuball=function(f,e,k,g){f=[];g=g/3-2;return f=f.concat(this.circle(k-g,e,g,g),this.circle(k-g,e+g+4,g,g),this.circle(k-g,e+2*(g+4),g,g))};P.prototype.renderExporting=function(){var f=this,e=f.options.exporting,k=e.buttons,g=f.isDirtyExporting||!f.exportSVGElements;f.buttonOffset=0;f.isDirtyExporting&&f.destroyExport();g&&!1!==e.enabled&&(f.exportEvents=[],f.exportingGroup=f.exportingGroup||f.renderer.g("exporting-group").attr({zIndex:3}).add(),H(k,function(n){f.addButton(n)}),f.isDirtyExporting=!1);m(f,"destroy",f.destroyExport)};m(P,"init",function(){var e=this;e.exporting={update:function(f,g){e.isDirtyExporting=!0;Q(!0,e.options.exporting,f);N(g,!0)&&e.redraw()}};U.addUpdate(function(f,g){e.isDirtyExporting=!0;Q(!0,e.options.navigation,f);N(g,!0)&&e.redraw()},e)});P.prototype.callbacks.push(function(e){e.renderExporting();m(e,"redraw",e.renderExporting)})});a(b,"masters/modules/exporting.src.js",[],function(){})});