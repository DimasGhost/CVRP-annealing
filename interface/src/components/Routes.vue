<template>
    <div class="routes">
        <div class="ui-label">Маршруты</div>
        <div class="content">
            <div class="routes-labels">
                <div class="route-label" v-for="route in routes" :key="route.id" :style="route.style">
                    {{route.id}}:
                </div>
            </div>
            <div class="vertexes">
                <div class="vertex" v-for="vertex in vertexes" :key="vertex.id" :style="vertex.style">
                    {{vertex.id}}
                </div>
            </div>
        </div>
    </div>
</template>

<style lang="scss" scoped>
    .routes {
        width: 315px;
    }

    .content {
        margin-top: 20px;
        display: flex;
    }

    .routes-labels {
        margin-left: 5px;
    }

    .route-label {
        color: white;
        font-size: 16px;
        padding-top: 2px;
        transition: all 0.7s ease;
    }

    .vertexes {
        margin-left: 10px;
        position: relative;
    }

    .vertex {
        position: absolute;
        width: 22px;
        height: 22px;
        font-size: 12px;
        color: white;
        background: #3469AF;
        display: flex;
        justify-content: center;
        align-items: center;
        border-radius: 3px;
        transition: all 0.7s ease;
    }
</style>

<script>

const V_IN_ROW = 11;

export default {
    data() {
        return {
            routes: [],
            vertexes: []
        }
    },
    mounted() {
        window.eventBus.$on('redraw', this.updateRoutes);
    },
    methods: {
        updateRoutes() {
            let routes = [];
            if ([
                stateEnum.SOLUTION_VIEW,
                stateEnum.SOLVING,
                stateEnum.PAUSED,
                stateEnum.SOLUTION_SAVING
            ].includes(window.globalStore.state)) {
                if (window.globalStore.showOnlyBest)
                    routes = window.globalStore.bestRoutes;
                else
                    routes = window.globalStore.currentRoutes;
            }

            while (this.routes.length > 0)
                this.routes.pop();
            while (this.vertexes.length > 0)
                this.vertexes.pop();

            let routeColors = ["#10A8AB", "#3469AF", "#4DBFF1", "#E64C66", "#FBB150"];
            let color = 0;

            let y = 0, rid = 0;
            for (let route of routes) {
                color = (color + 1) % routeColors.length;
                let rows = Math.floor(route.length / V_IN_ROW) + (route.length % V_IN_ROW > 0);
                this.routes.push({
                    id: ++rid,
                    style: {
                        height: `${rows * 25 + 5}px`
                    }
                });
                let x = 0, col = 1;
                for (let vertex of route) {
                    while (this.vertexes.length < vertex + 1)
                        this.vertexes.push({
                            id: this.vertexes.length + 1
                        });
                    this.vertexes[vertex].style = {
                        top: `${y}px`,
                        left: `${x}px`,
                        background: routeColors[color]
                    };
                    this.vertexes[0].style = { display: 'none' };
                    if (col === V_IN_ROW) {
                        col = 1;
                        x = 0;
                        y += 25;
                    } else {
                        col += 1;
                        x += 25;
                    }
                }
                if (route.length % V_IN_ROW > 0)
                    y += 25;
                y += 7;
            }
        }
    }
}
</script>