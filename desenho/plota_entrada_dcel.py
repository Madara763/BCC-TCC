import sys
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d.art3d import Poly3DCollection


def ler_malha(nome_arquivo):
    with open(nome_arquivo, "r") as arquivo:
        # Primeira linha: número de vértices e faces
        n, f = map(int, arquivo.readline().split())

        # Lê os vértices
        vertices = []

        for _ in range(n):
            x, y, z = map(int, arquivo.readline().split())
            vertices.append((x, y, z))

        # Lê as faces
        faces = []

        for _ in range(f):
            indices = list(map(int, arquivo.readline().split()))

            # O arquivo usa índices começando em 1.
            # Python usa índices começando em 0.
            face = [i - 1 for i in indices]

            faces.append(face)

    return vertices, faces


def desenhar_malha(vertices, faces):
    fig = plt.figure()
    ax = fig.add_subplot(111, projection="3d")

    # --------------------------------------------------
    # Desenha as faces
    # --------------------------------------------------

    poligonos = []

    for face in faces:
        pontos = [vertices[i] for i in face]
        poligonos.append(pontos)

    colecao = Poly3DCollection(
        poligonos,
        alpha=0.35,
        edgecolor="black"
    )

    ax.add_collection3d(colecao)

    # --------------------------------------------------
    # Desenha as arestas
    # --------------------------------------------------

    for face in faces:

        for i in range(len(face)):

            v1 = vertices[face[i]]
            v2 = vertices[face[(i + 1) % len(face)]]

            ax.plot(
                [v1[0], v2[0]],
                [v1[1], v2[1]],
                [v1[2], v2[2]],
                color="black"
            )

    # --------------------------------------------------
    # Desenha os vértices
    # --------------------------------------------------

    xs = [v[0] for v in vertices]
    ys = [v[1] for v in vertices]
    zs = [v[2] for v in vertices]

    ax.scatter(
        xs,
        ys,
        zs,
        color="red",
        s=50
    )

    # --------------------------------------------------
    # Mostra o índice de cada vértice
    # --------------------------------------------------

    for i, (x, y, z) in enumerate(vertices):

        ax.text(
            x,
            y,
            z,
            f"  {i + 1}",
            fontsize=10
        )

    # --------------------------------------------------
    # Configuração dos eixos
    # --------------------------------------------------

    ax.set_xlabel("X")
    ax.set_ylabel("Y")
    ax.set_zlabel("Z")

    ax.set_title("Malha 3D")

    # Mantém a mesma escala nos três eixos
    max_range = max(
        max(xs) - min(xs),
        max(ys) - min(ys),
        max(zs) - min(zs)
    ) / 2

    mid_x = (max(xs) + min(xs)) / 2
    mid_y = (max(ys) + min(ys)) / 2
    mid_z = (max(zs) + min(zs)) / 2

    ax.set_xlim(mid_x - max_range, mid_x + max_range)
    ax.set_ylim(mid_y - max_range, mid_y + max_range)
    ax.set_zlim(mid_z - max_range, mid_z + max_range)

    plt.show()


def main():

    if len(sys.argv) != 2:
        print(f"Uso: {sys.argv[0]} <arquivo>")
        sys.exit(1)

    nome_arquivo = sys.argv[1]

    vertices, faces = ler_malha(nome_arquivo)

    desenhar_malha(vertices, faces)


if __name__ == "__main__":
    main()
